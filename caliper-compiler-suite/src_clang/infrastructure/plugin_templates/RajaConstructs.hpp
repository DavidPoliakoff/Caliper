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
class RajaKernel
{
public:
  RajaKernel(const CallExpr* call, const LambdaExpr* lambda)
      : m_call(call), m_lambda(lambda)
  {
  }
  RajaKernel() : m_call(NULL), m_lambda(NULL) {}
  const CallExpr* getRajaCall() const { return m_call; }
  const LambdaExpr* getLambda() const { return m_lambda; }

private:
  const CallExpr* m_call;
  const LambdaExpr* m_lambda;
};
