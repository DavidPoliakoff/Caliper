//------------------------------------------------------------------------------
// plugin_print_funcnames Clang sample. Demonstrates:
//
// * How to create a Clang plugin.
// * How to use AST actions to access the AST of the parsed code.
//
// Once the .so is built, it can be loaded by Clang. For example:
//
// $ clang -cc1 -load build/plugin_print_funcnames.so -plugin print-fns <cfile>
//
// Taken from the Clang distribution. LLVM's license applies.
//------------------------------------------------------------------------------
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/ParentMap.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Index/USRGeneration.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

#include "infrastructure/llnl_static_analysis_utilities.h"

#include <regex>
#include <string>

using namespace clang;
namespace
{
using regexType = std::regex *;
class RajaDeclarationVisitor
    : public RecursiveASTVisitor<RajaDeclarationVisitor>
{
public:
  RajaDeclarationVisitor(ASTContext &Context, Rewriter *writer, regexType regex)
      : TheASTContext(Context), TheRewriter(writer), m_regex(regex)
  {
  }
  const CallExpr *getCallFromRajaLambda(const LambdaExpr *lambda)
  {
    return walkUpTo<CallExpr>(TheASTContext, lambda);
  }
  bool checkWrites(std::string varName, const LambdaExpr *lambda)
  {
    bool validWrite = true;
    for (auto stmt : lambda->getBody()->body()) {
      operateOnType<BinaryOperator>(stmt, [&](const BinaryOperator *oper) {
        operateOnType<ValueDecl>(
            getAssignedDeclOnLHS(oper), [&](const ValueDecl *refdVar) {
              if (std::regex_search(refdVar->getQualifiedNameAsString(),
                                    std::regex(".*" + varName + ".*"))) {
                validWrite = false;
              }
            });
      });
    }
    return validWrite;
  }

  const Expr *getLastAssignmentForCapture(
      const clang::LambdaCapture captureExpr,
      const LambdaExpr *expr)
  {
    const VarDecl *capturedVar = captureExpr.getCapturedVar();
    if (capturedVar) {
      auto variableAssignmentList = scanFuncBefore<Stmt>(
          TheASTContext, m_func, expr, [&](const Stmt *candidate) {
            bool isAssignment = false;
            assignmentType candidateAssignment = getAssignment(candidate);
            if (hasValidLHS(candidateAssignment)) {
              if (getLHS(candidateAssignment)->getQualifiedNameAsString()
                  == captureExpr.getCapturedVar()->getQualifiedNameAsString()) {

                isAssignment = true;
              }
            }
            return isAssignment;
          });
      if (!variableAssignmentList.empty()) {
        assignmentType lastAssignment =
            getAssignment(variableAssignmentList.back());
        if (hasValidLHS(lastAssignment)) {
          const Expr *assignmentExpr = getRHS(lastAssignment);
          return assignmentExpr;
        }
      }
    }
    return NULL;
  }
  using assignmentTuple = std::tuple<bool, SourceRange>;
  assignmentTuple verifyCaptureExpr(const clang::LambdaCapture capture,
                                    const LambdaExpr *lambda)
  {
    bool isValidCaptureExpr = true;
    const VarDecl *capturedVar = capture.getCapturedVar();
    SourceRange callLoc;
    if (capturedVar) {
      std::string capturedVariableName = capturedVar->getNameAsString();
      const Expr *lastMod = getLastAssignmentForCapture(capture, lambda);
      operateOnType<CallExpr>(validateCall(lastMod, *m_regex),
                              [&](const CallExpr *call) {
                                callLoc = call->getSourceRange();
                                isValidCaptureExpr =
                                    checkWrites(capturedVariableName, lambda);
                              });
      operateOnType<ConditionalOperator>(
          lastMod, [&](const ConditionalOperator *condition) {

            operateOnType<CallExpr>(validateCall(condition->getTrueExpr(),
                                                 *m_regex),
                                    [&](const CallExpr *call) {
                                      bool thisWriteSafe =
                                          checkWrites(capturedVariableName,
                                                      lambda);
                                      isValidCaptureExpr &= thisWriteSafe;
                                      if (!thisWriteSafe) {
                                        callLoc = call->getSourceRange();
                                      }
                                    });
            operateOnType<CallExpr>(validateCall(condition->getFalseExpr(),
                                                 *m_regex),
                                    [&](const CallExpr *call) {
                                      bool thisWriteSafe =
                                          checkWrites(capturedVariableName,
                                                      lambda);
                                      isValidCaptureExpr &= thisWriteSafe;
                                      if (!thisWriteSafe) {
                                        callLoc = call->getSourceRange();
                                      }
                                    });
          });
    }
    return std::make_tuple(isValidCaptureExpr, callLoc);
  }
  bool verifyCaptureSource(const LambdaExpr *lambda,
                           const clang::LambdaCapture capture)
  {
    assignmentTuple lastAssignment = verifyCaptureExpr(capture, lambda);
    if (!std::get<0>(lastAssignment)) {
      clang::SourceRange captureLoc = std::get<1>(lastAssignment);

      clang::SourceRange *lambdaLoc = new SourceRange(capture.getLocation());

      auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
      auto diagid1 =
          diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                     "Use of pointer directly returned by "
                                     "dangerous function in a RAJA lambda");
      SourceRange *lambdaDefLoc = new SourceRange(lambda->getLocStart());
      auto diagid2 =
          diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                     "Last assigned before capture here");
      // auto diagid3 = diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
      //                                          "Captured in this lambda");
      diagEngine.Report(captureLoc.getBegin(), diagid2)
          .AddSourceRange(CharSourceRange(captureLoc, false));
      ;
      diagEngine.Report(captureLoc.getBegin(), diagid2).addFlagValue("ARES");
      // assignmentDiag.AddSourceRange(CharSourceRange(captureLoc, false));
      // assignmentDiag.addFlagValue("ARES");
      // diagEngine.Report(lambdaDefLoc->getBegin(), diagid3)
      //    .AddSourceRange(CharSourceRange(*lambdaDefLoc, false));
      diagEngine.Report(lambdaLoc->getBegin(), diagid1)
          .AddSourceRange(CharSourceRange(captureLoc, false));
      diagEngine.Report(lambdaLoc->getBegin(), diagid1).addFlagValue("ARES");
    }
    return true;
  }
  bool shouldVisitTemplateInstantiations() const { return true; }
  bool verifyCapturedVariables(const LambdaExpr *lambda)
  {
    const CallExpr *rajaCall = getCallFromRajaLambda(lambda);
    if ((!rajaCall) || (!lambda)) {
      return true;
    }
    bool validCaptureVariable = true;
    operateOnType<FunctionDecl>(rajaCall->getCalleeDecl(),
                                [&](const FunctionDecl *decl) {
                                  for (const clang::LambdaCapture capture :
                                       lambda->captures()) {
                                    validCaptureVariable |=
                                        verifyCaptureSource(lambda, capture);
                                  }
                                });
    return validCaptureVariable;
  }
  bool VisitDecl(Decl *dcl)
  {
    // if this is a function declaration, mark my current function
    operateOnType<FunctionDecl>(dcl, [&](const FunctionDecl *func) {
      m_func = func;
    });
    return true;
  }
  bool VisitStmt(Stmt *stmt)
  {
    // if this is a lambda
    operateOnType<LambdaExpr>(stmt, [&](const LambdaExpr *lambda) {
      // verify those variables it captures
      verifyCapturedVariables(lambda);
    });
    // if this is a break statment
    operateOnType<BreakStmt>(stmt, [&](const BreakStmt *ret) {
      // check if it's in a lambda
      auto lambda = walkUpTo<LambdaExpr>(TheASTContext, ret);
      if (lambda) {
        auto retLoc = ret->getLocStart();
        auto lambdaLoc = lambda->getLocStart();
        auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
        auto diagid = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                                 "Break statement within "
                                                 "lambda expression");
        auto diagid2 = diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                                  "In this lambda");
        diagEngine.Report(retLoc, diagid);
        diagEngine.Report(lambdaLoc, diagid2);
      }
    });
    return true;
  }
  ASTContext &TheASTContext;
  Rewriter *TheRewriter;
  regexType m_regex;
  const FunctionDecl *m_func;
};

class RajaCudaCheckerConsumer : public ASTConsumer
{
public:
  RajaCudaCheckerConsumer(CompilerInstance &CI, regexType in_regex)
      : TheRewriter(new Rewriter()),
        Visitor(
            RajaDeclarationVisitor(CI.getASTContext(), TheRewriter, in_regex)),
        m_regex(in_regex)
  {
    TheRewriter->setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  }
  virtual void HandleTranslationUnit(ASTContext &context)
  {
    // const RewriteBuffer *RewriteBuf =
    //    &TheRewriter->getEditBuffer(TheRewriter->getSourceMgr().getMainFileID());
    Visitor.TraverseDecl(context.getTranslationUnitDecl());
    // if(RewriteBuf->size()){
    //    //llvm::outs() << std::string(RewriteBuf->begin(), RewriteBuf->end());
    //}
  }
  Rewriter *TheRewriter;
  RajaDeclarationVisitor Visitor;
  regexType m_regex;
};

class RajaCudaCheckAction : public PluginASTAction
{
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override
  {
    return llvm::make_unique<RajaCudaCheckerConsumer>(CI, m_regex);
  }
  virtual ActionType getActionType() override { return AddBeforeMainAction; }
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override
  {

    m_regex = new std::regex(".*", std::regex::optimize);
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      *m_regex = std::regex(args[i], std::regex::optimize);
      llvm::errs() << "PrintFunctionNames arg = " << args[i] << "\n";

      // Example error handling.
      if (args[i] == "-an-error") {
        DiagnosticsEngine &D = CI.getDiagnostics();
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "invalid argument '%0'");
        D.Report(DiagID) << args[i];
        return false;
      }
    }
    if (args.size() && args[0] == "help") PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream &ros)
  {
    ros << "Help for PrintFunctionNames plugin goes here\n";
  }
  // PluginASTAction::ActionType getActionType() override {
  //          return AddBeforeMainAction;
  //}
  regexType m_regex;
};
}  // end anonymous namespace

static FrontendPluginRegistry::Add<RajaCudaCheckAction> X("lambda-checker",
                                                          "print function "
                                                          "names");
