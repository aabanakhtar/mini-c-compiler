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

        // create binary
        AST::Literal lit(0,static_cast<int>(5));
        AST::Literal l2(0, static_cast<int>(15));
        const AST::ExprVariant p = std::make_unique<AST::Binary>(0, lit, TokenType::STAR, l2);

        llvm::Value *ret_val = generate(p);
        builder->CreateRet(ret_val);
        llvm::verifyFunction(*func);
        mod->print(llvm::outs(), nullptr);
    }   

    // useful visitor impl
    llvm::Value* generate(const AST::ExprVariant& e) 
    {
        static llvm::Value* last_visited; 
        std::visit([&](auto& x) { last_visited = this->gen(x); }, e);
        return last_visited;
    }

    llvm::Value* gen(const AST::Literal& lit);
    llvm::Value* gen(const AST::Variable& var);

    llvm::Value* gen(const std::unique_ptr<AST::Binary>& bin);
    llvm::Value* gen(const std::unique_ptr<AST::Unary>& un);
    llvm::Value* gen(const std::unique_ptr<AST::Assignment>& asn);
    llvm::Value* gen(const std::unique_ptr<AST::Call>& call);
    llvm::Value* gen(const std::unique_ptr<AST::StructAccess>& sa);
    llvm::Value* gen(const std::unique_ptr<AST::ArrayAccess>& aa);

    llvm::Value* generate_int_ops(const std::unique_ptr<AST::Binary>& bin);
    llvm::Value* generate_precise_ops(const std::unique_ptr<AST::Binary>& bin);

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