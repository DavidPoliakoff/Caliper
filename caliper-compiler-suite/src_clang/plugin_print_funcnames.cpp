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
#include "clang/AST/Mangle.h"
#include "llvm/Support/raw_ostream.h"
#include <set>
#ifndef GOTCHA_WRAPPER_PREFIX
  #define GOTCHA_WRAPPER_PREFIX "wrap_"
#endif
using namespace clang;

namespace
{

ASTContext* theContext;
struct function_rep{
  std::string retType;
  std::string parameterList;
  std::string pointerName;
  std::string name;
  std::string mangledName;
  std::vector<std::string> ptypeList;
  std::vector<std::string> pnameList;
};
std::set<std::string> knownSigs;
std::vector<function_rep> wrapped_func_names;

class PrintFunctionsConsumer : public ASTConsumer
{
public:
  virtual bool HandleTopLevelDecl(DeclGroupRef DG)
  {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
      const Decl *D = *i;
      if (const NamedDecl *ND = dyn_cast<NamedDecl>(D)){
      }
      if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)){
        function_rep nextRep;
        std::string ret = FD->getReturnType().getAsString();
        nextRep.retType = ret;
        std::string printMe = ret + " " + FD->getQualifiedNameAsString();
        std::string arg_string = "(";
        nextRep.name = FD->getQualifiedNameAsString();
        bool hasArgs = false;
        int argNum = 0;
        for(auto arg : FD->parameters()){
          nextRep.pnameList.push_back(arg->getNameAsString() == "" ? ("arg_"+argNum++) : arg->getNameAsString());
          nextRep.ptypeList.push_back(arg->getType().getAsString());
          arg_string += arg->getType().getAsString() + ",";
          hasArgs = true;
        }
        if(hasArgs){
          arg_string[arg_string.length()-1] = ')';
        }
        else{
          arg_string += ')';
        }
        if(theContext){
          auto mangler = theContext->createMangleContext();
          std::string mangledName;
          llvm::raw_string_ostream ostream(mangledName);
          if(mangler->shouldMangleDeclName(FD)){
            mangler->mangleName(FD, ostream);
            ostream.flush();
          }
          else{
            mangledName = FD->getQualifiedNameAsString();
          }
          nextRep.mangledName = mangledName;
          delete mangler;
        }
        std::string pointerName = ret + " (* orig_" + FD->getQualifiedNameAsString() + ")(";
        hasArgs = false;
        for(auto arg : FD->parameters()){
          pointerName += arg->getType().getAsString() + ",";
          hasArgs = true;
        }
        if(hasArgs){
          pointerName[pointerName.length()-1] = ')';
        }
        else{
          pointerName += ')';
        }
        printMe+= arg_string;
        nextRep.parameterList = arg_string;
        nextRep.pointerName = pointerName;
        if(knownSigs.find(pointerName)==knownSigs.end()){
          wrapped_func_names.push_back(nextRep);
          knownSigs.insert(pointerName);
        }
      }
    }
    return true;
  }
};

class PrintFunctionNamesAction : public PluginASTAction
{
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef)
  {
    theContext = &CI.getASTContext();
    return llvm::make_unique<PrintFunctionsConsumer>();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args)
  {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
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
  virtual void EndSourceFileAction() override {
    // Interface
    {
      std::string libStub = "#ifdef __cplusplus\n"
                            "extern \"C\" {\n"
                            "#endif\n"
                            "#include<gotcha/gotcha.h>\n"
                            "#ifdef __cplusplus\n"
                            "}\n"
                            "#endif\n";
      std::string numEntries = std::to_string(wrapped_func_names.size());
      bool inLoop = false;
      for(auto funcSymbolName : wrapped_func_names){
        libStub += funcSymbolName.pointerName+";\n";
      }
      for(auto funcSymbolName : wrapped_func_names){
        libStub+=funcSymbolName.retType+" wrap_"+funcSymbolName.name+"(";
        bool hitLoop = false;
        for(int index = 0 ; index<funcSymbolName.pnameList.size();index++){
          libStub+=" "+funcSymbolName.ptypeList[index]+" "+funcSymbolName.pnameList[index]+",";  
          hitLoop = true;
        }
        if(hitLoop){
          libStub[libStub.length()-1]=')';
        }
        else{
          libStub+=")";
        }
        libStub+=";\n";
      }
      libStub += "struct gotcha_binding_t gotcha_bindings[] = {"; 
      for(auto funcSymbolName : wrapped_func_names){
        inLoop = true;
        libStub+="\n                                        { ";
        libStub+="\""+funcSymbolName.mangledName+"\",";
        libStub+=GOTCHA_WRAPPER_PREFIX+funcSymbolName.name + ",";
        libStub+="&orig_"+funcSymbolName.name + "},";
      }
      if(inLoop){
        libStub[libStub.length()-1]='\n';
        libStub+="                                      }";
      }
      else{
        libStub+="}";
      }
      libStub+="\n";
      libStub += "void handle_gotcha_actions ();\n";
      llvm::outs() << libStub ;
    }
    llvm::outs() << "/////////////////////////////////////////////\n";
    llvm::outs() << "/////////////////////////////////////////////\n";
    llvm::outs() << "/////////////////////////////////////////////\n";
    // Implementation
    {
      std::string libStub = "#ifdef __cplusplus\n"
                            "extern \"C\" {\n"
                            "#endif\n"
                            "#include<my_header.h>\n"
                            "#ifdef __cplusplus\n"
                            "}\n"
                            "#endif\n";
      std::string numEntries = std::to_string(wrapped_func_names.size());
      for(auto funcSymbolName : wrapped_func_names){
        libStub+=funcSymbolName.retType+" wrap_"+funcSymbolName.name+"(";
        bool hitLoop = false;
        for(int index = 0 ; index<funcSymbolName.pnameList.size();index++){
          libStub+=" "+funcSymbolName.ptypeList[index]+" "+funcSymbolName.pnameList[index]+",";  
          hitLoop = true;
        }
        if(hitLoop){
          libStub[libStub.length()-1]=')';
        }
        else{
          libStub+=")";
        }
        libStub+="{\n";
        libStub+= "  return orig_"+funcSymbolName.name+"(";
        hitLoop=false;
        for(auto x : funcSymbolName.pnameList){
          hitLoop = true;
          libStub+=x+",";
        }
        if(hitLoop){
          libStub[libStub.length()-1]=')';
        }
        else{
          libStub+=")";
        }
        libStub+=";\n";
         
        libStub+="}\n";
      }
      libStub += "void handle_gotcha_actions () {\n";
      libStub += "  gotcha_wrap(gotcha_bindings, sizeof(gotcha_bindings)/sizeof(struct gotcha_binding_t));\n";
      libStub+="}\n";
      llvm::outs()<<libStub<<"\n";
      
    }
  }
};
}

static FrontendPluginRegistry::Add<PrintFunctionNamesAction> X("print-fns",
                                                               "print function "
                                                               "names");
