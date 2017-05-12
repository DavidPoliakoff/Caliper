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

#include "../llnl_static_analysis_utilities.h"
#include "RajaConstructs.hpp"

#include <regex>
#include <string>

using namespace clang;
namespace
{
using regexType = std::regex *;
template <typename Derived>
class RajaVisitor : public RecursiveASTVisitor<RajaVisitor<Derived>>
{
public:
  void warnOutsideUse(clang::SourceRange captureLoc, clang::SourceRange useLoc)
  {
    auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
    auto diagid1 =
        diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                   "Use of pointer outside of RAJA "
                                   "intended for use only in RAJA, "
                                   "may trigger data movement");
    auto diagid2 =
        diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                   "Last assigned before capture here");
    diagEngine.Report(captureLoc.getBegin(), diagid2)
        .AddSourceRange(CharSourceRange(captureLoc, false));
    diagEngine.Report(useLoc.getBegin(), diagid1)
        .AddSourceRange(CharSourceRange(useLoc, false));
  }
  void warnCapture(clang::SourceRange captureLoc, clang::SourceRange useLoc)
  {
    auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
    auto diagid1 =
        diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                   "Use of pointer directly returned by "
                                   "dangerous function in a RAJA lambda");
    auto diagid2 =
        diagEngine.getCustomDiagID(DiagnosticsEngine::Note,
                                   "Last assigned before capture here");
    diagEngine.Report(captureLoc.getBegin(), diagid2)
        .AddSourceRange(CharSourceRange(captureLoc, false));
    diagEngine.Report(useLoc.getBegin(), diagid1)
        .AddSourceRange(CharSourceRange(useLoc, false));
  }
  void warnThis(clang::SourceRange useLoc, clang::SourceRange lambdaLoc)
  {
    auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
    auto diagid1 = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                              "Implicit use of 'this' inside "
                                              "RAJA kernel, "
                                              "consider only grabbing necessary "
                                              "parts of object in an outer scope");
    diagEngine.Report(useLoc.getBegin(), diagid1)
        .AddSourceRange(CharSourceRange(useLoc, false));
  }
  void warnMember(clang::SourceRange useLoc, clang::SourceRange lambdaLoc)
  {
    auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
    auto diagid1 = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                              "Access to struct member inside "
                                              "RAJA kernel, "
                                              "consider moving struct access "
                                              "to outer scope");
    diagEngine.Report(useLoc.getBegin(), diagid1)
        .AddSourceRange(CharSourceRange(useLoc, false));
  }
  RajaVisitor(ASTContext &Context, Rewriter *writer, regexType regex)
      : TheASTContext(Context), TheRewriter(writer), m_regex(regex)
  {
  }
  Derived &getDerived() { return *static_cast<Derived *>(this); }
  const CallExpr *getCallFromRajaLambda(const LambdaExpr *lambda)
  {
    return walkUpTo<CallExpr>(TheASTContext, lambda);
  }
  bool shouldVisitTemplateInstantiations() const { return true; }
  bool nameMatches(const NamedDecl *decl1, const NamedDecl *decl2)
  {
    std::string decl1name = "None";
    if (decl1) {
      decl1name = decl1->getQualifiedNameAsString();
    }

    std::string decl2name = "None";
    if (decl2) {
      decl2name = decl2->getQualifiedNameAsString();
    }
    return ((decl1name != "None") || (decl2name != "None"))
           && (decl1name == decl2name);
  }
  bool VisitDecl(Decl *dcl)
  {
    // if this is a function declaration, mark my current function
    operateOnType<FunctionDecl>(dcl, [&](const FunctionDecl *func) {
      m_func = func;
    });
    return true;
  }
  bool functionNameIsRajaRelated(std::string name)
  {
    return std::regex_search(name, std::regex(".*for_*all.*"))
           || std::regex_search(name, std::regex("RAJA::"));
  }
  bool functionInRajaNameSpace(std::string name)
  {
    //TODO: FIX THIS
    return true || std::regex_search(name, std::regex("RAJA::"));
  }
  bool isRajaWrapper(const CallExpr *call)
  {
    auto functionBody = getFunctionBody(call->getDirectCallee());
    for (auto &&statement : explodeFunctionBody(functionBody, 2)) {
      if (operateOnType<CallExpr>(
              statement, [&](const CallExpr *candidateCall) {
                return candidateCall && candidateCall->getDirectCallee()
                       && functionInRajaNameSpace(
                              candidateCall->getDirectCallee()
                                  ->getQualifiedNameAsString());
              })) {
        return true;
      }
    }
    return false;
  }
  // TODO: This is quite brittle
  // 1) Within RAJA, a lambda which isn't a RAJA lambda
  // 2) Double counting RAJA statements, a VisitRajaKernel and VisitNonRajaStmt
  // trigger
  // 3) Nested RAJA
  bool TraverseStmt(Stmt *stmt)
  {
    if (stmt) {
      operateOnType<LambdaExpr>(stmt, [&](const LambdaExpr *lambda) {
        if (lambda) {
          const CallExpr *rajaCall = getCallFromRajaLambda(lambda);
          if (rajaCall) {
            if (!isRajaWrapper(rajaCall)) {

              return true;
            }
            m_kernel = RajaKernel(rajaCall, lambda);
            inRajaContext = true;
            // lambda->dumpColor();
          }
        }
        return true;
      });
    }
    bool shouldContinue = true;
    if (!stmt || !operateOnType<CUDAKernelCallExpr>(stmt)) {
      shouldContinue =
          RecursiveASTVisitor<RajaVisitor<Derived>>::TraverseStmt(stmt);
    } else {
      return false;
    }
    inRajaContext = false;
    return shouldContinue;
  }
  bool VisitNonRajaStmt(Stmt *stmt) { return true; }
  bool inRaja(const Expr *stm) { return inRajaContext; }
  bool VisitStmt(Stmt *stmt)
  {
    operateOnType<LambdaExpr>(stmt, [&](const LambdaExpr *lambda) {
      if (lambda) {
        if (inRajaContext) {
          return getDerived().VisitRajaKernel(&m_kernel);
        }
      }
      return true;
    });
    getDerived().VisitNonRajaStmt(stmt);
    return true;
  }
  const FunctionDecl *getEnclosingFunction() const { return m_func; }
  child_vec<const Stmt *> getPrologue()
  {
    return child_vec<const Stmt*>(scanFuncBefore<Stmt>(TheASTContext,
                                getEnclosingFunction(),
                                m_kernel.getLambda()));
  }
  using captureListType = decltype(RajaKernel().getLambda()->captures());
  captureListType getCaptures() const
  {
    return m_kernel.getLambda()->captures();
  }
  ASTContext &TheASTContext;
  Rewriter *TheRewriter;
  RajaKernel m_kernel;
  regexType m_regex;
  const FunctionDecl *m_func;
  const CallExpr *m_raja_call;
  bool inRajaContext;
};
}  // end namespace clang
