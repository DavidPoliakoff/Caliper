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
class StructCheckerVisitor : public RajaVisitor<StructCheckerVisitor>
{
public:
  StructCheckerVisitor(ASTContext &Context, Rewriter *writer, regexType regex)
      : RajaVisitor<StructCheckerVisitor>(Context, writer, regex)
  {
  }
  bool VisitNonRajaStmt(Stmt *stmt)
  {
    operateOnType<MemberExpr>(stmt, [&](const MemberExpr *deref) {
      if (!deref || !inRaja(deref)) {
        return;
      } else {
        if (m_kernel.getLambda()) {
          warnMember(deref->getLocStart(), m_kernel.getLambda()->getLocStart());
        }
      }
    });
    return true;
  }
  bool VisitRajaKernel(const RajaKernel *kernel)
  {
    // across captured variables
    return true;
  }
};
}  // end anonymous namespace

static FrontendPluginRegistry::Add<RegexVisitorScaffolding<StructCheckerVisitor>::AddTemplateType> X("struct-checker",
                                                          "print function "
                                                          "names");
