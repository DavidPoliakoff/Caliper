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

#include <regex>
#include <set>
#include <fstream>
#include <iterator>
#include "infrastructure/llnl_static_analysis_utilities.h"

using namespace clang;

namespace
{

static std::set<std::string> parsedFiles;
std::ofstream outfile;

class CaliperInstrumentationConsumer : public ASTConsumer
{
public:
  ~CaliperInstrumentationConsumer(){
    for(auto item: parsedFiles){
      outfile << item << "\n";
    }
    outfile.close();
  }
  CaliperInstrumentationConsumer(CompilerInstance &CI, std::regex &in_regex) : ASTConsumer()
  {
    TheRewriter = Rewriter();
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    m_regex = in_regex;
    std::ifstream ifile("instrumentation_log");
    std::vector<std::string> parsedFileVec;
    std::copy(std::istream_iterator<std::string>(ifile),
              std::istream_iterator<std::string>(),
              std::back_inserter(parsedFileVec)
        );
    parsedFiles = std::move(std::set<std::string>(parsedFileVec.begin(),parsedFileVec.end()));
    ifile.close();
    outfile.open("instrumentation_log");
  }
  std::string getCaliStartAnnotation(std::string name){
    return std::string("if (cali_function_attr_id == CALI_INV_ID)\n " \
            " cali_init();\n " \
             " cali_begin_string(cali_function_attr_id, \"") + name + std::string("\");\n");
  }
  template<typename handled_type, typename modifier>
  void modify_recursively(const Stmt* modified, modifier mod){
    operateOnType<handled_type>(modified, [&](const handled_type* stm){
        mod(stm);
    });
    if(modified){
      operateOnType<Stmt>(modified, [&] (const Stmt* checked_modified){
        std::vector<const Stmt*> rev_vec;
        llvm::outs() << checked_modified << "\n";
        if((checked_modified!=0x0)){
          if((checked_modified->children().begin() !=checked_modified->children().end())){
            for(auto* child : checked_modified->children()){
              if(child){
                rev_vec.push_back(child);
              }
            };
            for(auto* child : rev_vec){
              modify_recursively<ReturnStmt>(child, mod);
            }
          }
        }
      });
    }
  }
  FileID last_file;
  virtual bool HandleTopLevelDecl(DeclGroupRef DG)
  {
    alter_methods(DG, [&](const FunctionDecl *in) {
      if(in && in->hasBody()){
        
        if (std::regex_search(in->getQualifiedNameAsString(), m_regex) && !parsedFiles.count(TheRewriter.getSourceMgr().getFilename(in->getLocStart()))) {
          parsedFiles.insert(TheRewriter.getSourceMgr().getFilename(in->getLocStart())); //FLAG FN

          auto declFileID = TheRewriter.getSourceMgr().getFileID(in->getLocStart());
          if(declFileID==last_file){
            TheRewriter.InsertTextBefore(in->getLocStart()," #include<caliper/cali.h>\n "
                                                           );
          }
          modify_recursively<ReturnStmt>(in->getBody(), [&]( const ReturnStmt* ret){
            TheRewriter.InsertTextBefore(ret->getLocStart(),"cali_end(cali_function_attr_id);\n");
          });
          auto firstStatementChildVec = in->getBody()->children();
          if(firstStatementChildVec.begin()!=firstStatementChildVec.end()){
            auto firstStatement = firstStatementChildVec.begin();
            Rewriter* theRet = new Rewriter();
            TheRewriter.InsertTextBefore(firstStatement->getSourceRange().getBegin(),getCaliStartAnnotation(in->getNameAsString()));
            TheRewriter.InsertTextBefore(in->getBodyRBrace(),"cali_end(cali_function_attr_id);\n");
          }
        }
      }
    });

    return true;
  }
  virtual void HandleTranslationUnit(ASTContext &Ctx)
  {
    last_file = Ctx.getSourceManager().getMainFileID();
    if(last_file.isValid()){
      //TheRewriter.overwriteChangedFiles();
       Rewriter* ret;
       //
       //llvm::outs() << Ctx.getSourceManager().getFileEntryForID(last_file)->tryGetRealPathName() <<"\n";
      const RewriteBuffer *RewriteBuf =
          TheRewriter.getRewriteBufferFor(last_file);
      if (RewriteBuf->size()) {
        llvm::outs() << std::string(RewriteBuf->begin(), RewriteBuf->end());
      }
    }
    // TheRewriter.getRewriteBufferFor(TheRewriter.getSourceMgr().getMainFileID());
  }
  Rewriter TheRewriter;
  std::regex m_regex;
};

class CaliperInstrumentationAction : public PluginASTAction
{
protected:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override
  {
    return llvm::make_unique<CaliperInstrumentationConsumer>(CI, m_regex);
  }
  virtual ActionType getActionType() override { return AddBeforeMainAction; }
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override
  {
    m_regex = std::regex(".*", std::regex::optimize);
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      m_regex = std::regex(args[i], std::regex::optimize);
      // llvm::errs() << "PrintFunctionNames arg = " << args[i] << "\n";

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
  std::regex m_regex;
};
}  // end anonymous namespace

static FrontendPluginRegistry::Add<CaliperInstrumentationAction> X(
    "caliper-instrument",
    "print function names");
