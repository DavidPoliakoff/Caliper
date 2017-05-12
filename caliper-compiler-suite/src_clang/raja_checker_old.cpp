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
  std::string getPolicyName(const CallExpr *call)
  {
    std::string type = "NONRAJA";
    operateOnType<FunctionDecl>(call->getCalleeDecl(),
                                [&](const FunctionDecl *decl) {
                                  type = decl->getTemplateSpecializationArgs()
                                             ->get(0)
                                             .getAsType()
                                             .getAsString();
                                });
    return type;
  }
  bool typeMatches(std::string name, std::string regex)
  {
    return std::regex_search(name, std::regex(regex));
  }
  bool verifyCaptureReductionSize(const LambdaExpr *lambda,
                                  const clang::LambdaCapture capture,
                                  llvm::APSInt reductionSize)
  {
    auto capturedType = capture.getCapturedVar()->getType();
    auto specialization = capturedType->getAsCXXRecordDecl();
    bool isCorrectReduction = false;
    SourceRange *lambdaDefLoc = new SourceRange(lambda->getLocStart());
    operateOnType<ClassTemplateSpecializationDecl>(
        specialization,
        [&](const ClassTemplateSpecializationDecl *templateSpec) {
          isCorrectReduction = true;
          auto blockSize =
              templateSpec->getTemplateArgs().get(1).getAsIntegral();
          std::string blockSizeAsString =
              std::to_string(blockSize.getExtValue());
          if (blockSize != reductionSize) {
            isCorrectReduction = false;
            clang::SourceRange *captureLoc =
                new SourceRange(capture.getCapturedVar()->getSourceRange());
            clang::SourceRange *lambdaLoc =
                new SourceRange(capture.getLocation());
            auto &diagEngine =
                TheASTContext.getSourceManager().getDiagnostics();
            auto diagid =
                diagEngine.getCustomDiagID(DiagnosticsEngine::Error,
                                           "Reducer block size incompatible "
                                           "with RAJA forall");
            auto diagid2 = diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                                      "Due to use here");
            auto diagid3 =
                diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                           "Captured in this lambda");
            diagEngine.Report(captureLoc->getBegin(), diagid)
                .AddSourceRange(CharSourceRange(*captureLoc, false));
            diagEngine.Report(lambdaDefLoc->getBegin(), diagid3)
                .AddSourceRange(CharSourceRange(*captureLoc, false));
            diagEngine.Report(lambdaLoc->getBegin(), diagid2)
                .AddSourceRange(CharSourceRange(*captureLoc, false));
          }
        });
    return isCorrectReduction;
  }
  bool checkWrites(std::string varName, const LambdaExpr *lambda)
  {
    bool isGood = true;
    for (auto stmt : lambda->getBody()->body()) {
      operateOnType<BinaryOperator>(stmt, [&](const BinaryOperator *oper) {
        operateOnType<ValueDecl>(
            getDeclFromLHS(oper), [&](const ValueDecl *refdVar) {
              if (std::regex_search(refdVar->getQualifiedNameAsString(),
                                    std::regex(".*" + varName + ".*"))) {
                isGood = false;
              }
            });
      });
      // operateOnType<ConditionalOperator>(stmt, [&] (const
      // ConditionalOperator* oper){
      //    operateOnType<ValueDecl>(getDeclFromLHS(oper),[&] (const ValueDecl*
      //    refdVar){
      //        if(std::regex_search(refdVar->getQualifiedNameAsString(),std::regex(".*"+varName+".*"))){
      //            isGood=false;
      //        }
      //    });
      //});
    }
    return isGood;
  }
  bool isLambda(const FunctionDecl *candidate)
  {
    return candidate && candidate->isCXXClassMember()
           && operateOnType<CXXMethodDecl>(
                  candidate, [&](const CXXMethodDecl *candidate) {
                    return candidate->getParent()->isLambda();
                  });
  }

  bool isRajaFunction(const FunctionDecl *candidate)
  {
    llvm::errs() << candidate->getQualifiedNameAsString() << "\n";
    return candidate
           && !std::regex_search(candidate->getQualifiedNameAsString(),
                                 std::regex(".*RAJA::.*forall.*"));
  }

  const Expr *getLastModification(const clang::LambdaCapture capturedVar,
                                  const LambdaExpr *expr)
  {
    const VarDecl *initStat = capturedVar.getCapturedVar();
    if (initStat) {
      auto preAssigns = scanFuncBefore<Stmt>(
          TheASTContext, m_func, expr, [&](const Stmt *candidate) {
            bool isAssignment = false;
            assignmentType candidateAssignment = getAssignment(candidate);
            if (isValid(candidateAssignment)) {
              if (getLHS(candidateAssignment)->getQualifiedNameAsString()
                  == capturedVar.getCapturedVar()->getQualifiedNameAsString()) {

                isAssignment = true;
              }
            }
            return isAssignment;
          });
      if (!preAssigns.empty()) {
        assignmentType lastAssignment = getAssignment(preAssigns.back());
        if (isValid(lastAssignment)) {
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
    bool isGood = true;
    const VarDecl *capturedVar = capture.getCapturedVar();
    SourceRange callLoc;
    if (capturedVar) {
      std::string varName = capturedVar->getNameAsString();
      const Expr *lastMod = getLastModification(capture, lambda);
      operateOnType<CallExpr>(validateCall(lastMod, *m_regex),
                              [&](const CallExpr *call) {
                                callLoc = call->getSourceRange();
                                isGood = checkWrites(varName, lambda);
                              });
      operateOnType<ConditionalOperator>(
          lastMod, [&](const ConditionalOperator *condition) {

            operateOnType<CallExpr>(validateCall(condition->getTrueExpr(),
                                                 *m_regex),
                                    [&](const CallExpr *call) {
                                      bool thisWriteSafe =
                                          checkWrites(varName, lambda);
                                      isGood &= thisWriteSafe;
                                      if (!thisWriteSafe) {
                                        callLoc = call->getSourceRange();
                                      }
                                    });
            operateOnType<CallExpr>(validateCall(condition->getFalseExpr(),
                                                 *m_regex),
                                    [&](const CallExpr *call) {
                                      bool thisWriteSafe =
                                          checkWrites(varName, lambda);
                                      isGood &= thisWriteSafe;
                                      if (!thisWriteSafe) {
                                        callLoc = call->getSourceRange();
                                      }
                                    });
          });
    }
    return std::make_tuple(isGood, callLoc);
  }
  bool verifyCaptureSource(const LambdaExpr *lambda,
                           const clang::LambdaCapture capture)
  {
    auto capVar = capture.getCapturedVar();
    assignmentTuple lastAssignment = verifyCaptureExpr(capture, lambda);
    if (!std::get<0>(lastAssignment)) {
      clang::SourceRange captureLoc = std::get<1>(lastAssignment);

      clang::SourceRange *lambdaLoc = new SourceRange(capture.getLocation());

      auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
      auto diagid =
          diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                     "Use of pointer directly returned by "
                                     "dangerous function in a RAJA lambda");
      SourceRange *lambdaDefLoc = new SourceRange(lambda->getLocStart());
      auto diagid2 =
          diagEngine.getCustomDiagID(DiagnosticsEngine::Note, "From use here");
      auto diagid3 = diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                                "Captured in this lambda");
      diagEngine.Report(captureLoc.getBegin(), diagid)
          .AddSourceRange(CharSourceRange(captureLoc, false));
      diagEngine.Report(lambdaDefLoc->getBegin(), diagid3)
          .AddSourceRange(CharSourceRange(*lambdaDefLoc, false));
      diagEngine.Report(lambdaLoc->getBegin(), diagid2)
          .AddSourceRange(CharSourceRange(captureLoc, false));
    }
    return true;
  }
  bool verifyChaify(const LambdaExpr *lambda,
                    const clang::LambdaCapture capture)
  {
    const QualType capturedType = capture.getCapturedVar()->getType();
    if (capturedType->isPointerType() || capturedType->isArrayType()) {
      clang::SourceRange *captureLoc =
          new SourceRange(capture.getCapturedVar()->getSourceRange());
      clang::SourceRange *lambdaLoc = new SourceRange(capture.getLocation());
      auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
      auto diagid =
          diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                     "Use of raw pointer/array type: did you "
                                     "mean to use a ManagedArray?");
      SourceRange *lambdaDefLoc = new SourceRange(lambda->getLocStart());
      auto diagid2 =
          diagEngine.getCustomDiagID(DiagnosticsEngine::Note, "From use here");
      auto diagid3 = diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                                "Captured in this lambda");
      diagEngine.Report(captureLoc->getBegin(), diagid)
          .AddSourceRange(CharSourceRange(*captureLoc, false));
      diagEngine.Report(lambdaDefLoc->getBegin(), diagid3)
          .AddSourceRange(CharSourceRange(*lambdaDefLoc, false));
      diagEngine.Report(lambdaLoc->getBegin(), diagid2)
          .AddSourceRange(CharSourceRange(*captureLoc, false));
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
    operateOnType<FunctionDecl>(dcl, [&](const FunctionDecl *func) {
      m_func = func;
    });
    return true;
  }
  bool VisitStmt(Stmt *stmt)
  {
    operateOnType<LambdaExpr>(stmt, [&](const LambdaExpr *lambda) {
      verifyCapturedVariables(lambda);
    });
    operateOnType<BreakStmt>(stmt, [&](const BreakStmt *ret) {
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
