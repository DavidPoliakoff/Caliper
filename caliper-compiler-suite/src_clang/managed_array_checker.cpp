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
#include "infrastructure/plugin_templates/RajaVisitor.hpp"
#include "infrastructure/plugin_templates/Consumer.hpp"

#include <regex>
#include <string>

using namespace clang;
namespace
{
using regexType = std::regex *;
class SampleRajaVisitor : public RajaVisitor<SampleRajaVisitor>
{
public:
  SampleRajaVisitor(ASTContext &Context, Rewriter *writer, regexType regex)
      : RajaVisitor<SampleRajaVisitor>(Context, writer, regex)
  {
  }
  void warnManagedArrayUse(clang::SourceRange useLoc)
  {
    auto &diagEngine = TheASTContext.getSourceManager().getDiagnostics();
    auto diagid1 =
        diagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                   "Use of ManagedArray outside of RAJA, "
                                   "intended for use only in RAJA, "
                                   "may be incorrect usage"
                                   );
    diagEngine.Report(useLoc.getBegin(), diagid1)
        .AddSourceRange(CharSourceRange(useLoc, false));
  }
  bool VisitNonRajaStmt(Stmt *stmt)
  {
    operateOnType<CXXOperatorCallExpr>(stmt, [&](const CXXOperatorCallExpr* candidate){
       auto lhs = candidate->getArg(0);
       operateOnType<DeclRefExpr>(lhs, [=] (const DeclRefExpr* referred){
          operateOnType<ValueDecl>(referred->getDecl(), [=](const ValueDecl* typedDecl){
             std::string declName = typedDecl->getType().getAsString();
             auto maybeLambda = walkUpTo<LambdaExpr>(TheASTContext,candidate);
             if(!maybeLambda){
                 auto thisLoc = candidate->getLocStart().printToString(TheASTContext.getSourceManager());
                 if(!std::regex_search(thisLoc,std::regex(".*/gcc/.*"))){
                   if(std::regex_search(declName,std::regex(".*ManagedArray.*"))){
                     warnManagedArrayUse(candidate->getLocStart());
                   }          
                 }
             } 
          });
       });
    });
    return true;
  }
  bool VisitRajaKernel(const RajaKernel *kernel)
  {
    return true;
  }
};
}  // end anonymous namespace

static FrontendPluginRegistry::Add<RegexVisitorScaffolding<SampleRajaVisitor>::AddTemplateType> X("managed-array-checker",
                                                          "print function "
                                                          "names");
