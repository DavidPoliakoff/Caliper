#pragma once
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

#include <regex>
#include <string>

using namespace clang;
namespace
{
using regexType = std::regex *;
template<typename VisitorType>
class RegexConsumer : public ASTConsumer {
public:
  RegexConsumer(CompilerInstance &CI, regexType in_regex)
      : TheRewriter(new Rewriter()),
        Visitor(VisitorType(CI.getASTContext(), TheRewriter, in_regex)),
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
  VisitorType Visitor;
  regexType m_regex;
}; //class RegexConsumer

template<typename ConsumerType>
class RegexAction : public PluginASTAction
{
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override
  {
    return llvm::make_unique<ConsumerType>(CI, m_regex);
  }
  virtual ActionType getActionType() override { return AddBeforeMainAction; }
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override
  {

    m_regex = new std::regex(".*", std::regex::optimize);
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      *m_regex = std::regex(args[i], std::regex::optimize);
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
  regexType m_regex;
};
template<typename VisitorType>
class RegexVisitorScaffolding{
public:
    using ConsumerType = RegexConsumer<VisitorType>;
    using ActionType = RegexAction<ConsumerType>;
    using AddTemplateType = ActionType;
};

} //end anonymous namespace
