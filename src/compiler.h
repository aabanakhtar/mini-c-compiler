#ifndef COMPILER_H
#define COMPILER_H

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "ast.h"

class Codegen
{
public:
    //Codegen(llvm::LLVMContext& context, llvm::Module& module, llvm::IRBuilder<>& builder);
        //: context(context), module(module), builder(builder) {}

    llvm::Value* generate_expr(const AST::ExprVariant& expr);

private:
    //llvm::LLVMContext& context;
    //llvm::Module& module;
    //llvm::IRBuilder<>& builder;

    llvm::Value* gen_literal(const AST::Literal &lit);
    llvm::Value* gen_variable(const AST::Variable& var);
    llvm::Value* gen_binary(const AST::Binary& bin);
    llvm::Value* gen_unary(const AST::Unary& un);
    llvm::Value* gen_assignment(const AST::Assignment& asn);
    llvm::Value* gen_call(const AST::Call& call);
    llvm::Value* gen_struct_access(const AST::StructAccess& sa);
    llvm::Value* gen_array_access(const AST::ArrayAccess& aa);

private: 
    static llvm::Module* mod; 
    static llvm::IRBuilder()

};


#endif // COMPILER_H