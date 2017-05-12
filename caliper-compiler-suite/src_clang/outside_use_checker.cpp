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
class UseOutsideRajaVisitor : public RajaVisitor<UseOutsideRajaVisitor>
{
public:
  UseOutsideRajaVisitor(ASTContext &Context, Rewriter *writer, regexType regex)
      : RajaVisitor<UseOutsideRajaVisitor>(Context, writer, regex)
  {
  }
  bool VisitNonRajaStmt(Stmt *stmt)
  {
    operateOnType<DeclRefExpr>(stmt, [&](const DeclRefExpr *deref) {
      operateOnType<VarDecl>(deref->getDecl(), [&](const VarDecl* refdVar){
        std::vector<const Stmt*> previousExprs = scanFuncBefore<Stmt>(TheASTContext, getEnclosingFunction(), deref, [=](const Stmt* prologueStatement){
          // get that statement as an assignment
          SourceLocation useRange = deref->getLocation();
          assignmentType candidateAssignment = getAssignment(prologueStatement);
          // if it has a valid assignment which has the same name as the captured
          // variable
          // (if this is the variable being captured)
          if (hasValidLHS(candidateAssignment)
              && nameMatches(getLHS(candidateAssignment),
                             refdVar)) {
            operateOnType<CallExpr>(
                getRHS(candidateAssignment), [&](const CallExpr *assignedToExpr) {
                  // if it is one, and the name of the called function matches
                  // the regex we were told to look for then we have a problem
                  if (std::regex_search(getNameOfCalled(assignedToExpr), *m_regex)) {
                    warnCapture(assignedToExpr->getSourceRange(), useRange);
                  }
                });
                // get the right hand side as a conditional expression
                // a conditional expression has the form "foo = condition ? something :
                // somethingElse"
                operateOnType<ConditionalOperator>(
                    getRHS(candidateAssignment), [&](const ConditionalOperator *conditional) {
                      // do the same filtering on its true and false branches
                      operateOnType<CallExpr>(
                          conditional->getTrueExpr(),
                          [&](const CallExpr *assignedToExpr) {
                            if (std::regex_search(getNameOfCalled(assignedToExpr),
                                                  *m_regex)) {
                              warnCapture(assignedToExpr->getSourceRange(), useRange);
                            }
                          });
                      operateOnType<CallExpr>(
                          conditional->getFalseExpr(),
                          [&](const CallExpr *assignedToExpr) {
                            if (std::regex_search(getNameOfCalled(assignedToExpr),
                                                  *m_regex)) {
                              warnCapture(assignedToExpr->getSourceRange(), useRange);
                            }
                          });
                    });
            
          }
          return true;
        });
      });
    });
    return true;
  }
  bool VisitRajaKernel(RajaKernel* kernel){
    return true;
  }
}; //class UseOutsideRajaVisitor
}  // end anonymous namespace

static FrontendPluginRegistry::Add<RegexVisitorScaffolding<UseOutsideRajaVisitor>::AddTemplateType> X("use-checker",
                                                          "print function "
                                                          "names");
