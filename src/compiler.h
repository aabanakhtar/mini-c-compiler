#ifndef COMPILER_H
#define COMPILER_H

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "ast.h"
#include <unordered_map>
#include <memory>

class Codegen
{
public:
    explicit Codegen();

    void test_literal_codegen()
    {
        llvm::FunctionType *func_type = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(*context), false);
        llvm::Function *func = llvm::Function::Create(
            func_type, llvm::Function::ExternalLinkage, "test", mod.get());
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context, "entry", func);
        builder->SetInsertPoint(bb);
        AST::Literal lit(0, int(5));
        llvm::Value *ret_val = gen_literal(lit);
        builder->CreateRet(ret_val);
        llvm::verifyFunction(*func);
        mod->print(llvm::outs(), nullptr);
    }   

    llvm::Value* generate_expr(const AST::ExprVariant& expr);

    llvm::Value* gen_literal(const AST::Literal &lit);
    llvm::Value* gen_variable(const AST::Variable& var);
    llvm::Value* gen_binary(const AST::Binary& bin);
    llvm::Value* gen_unary(const AST::Unary& un);
    llvm::Value* gen_assignment(const AST::Assignment& asn);
    llvm::Value* gen_call(const AST::Call& call);
    llvm::Value* gen_struct_access(const AST::StructAccess& sa);
    llvm::Value* gen_array_access(const AST::ArrayAccess& aa);

private: 
    // stores internal info that we just pass around
    std::unique_ptr<llvm::LLVMContext> context; 
    // creates ir instructions
    std::unique_ptr<llvm::IRBuilder<>> builder; 
    // the "translation unit"
    std::unique_ptr<llvm::Module> mod; 
    // keep track of stuff
    std::unordered_map<std::string, llvm::Value*> names; 
};


#endif // COMPILER_H