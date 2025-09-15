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

    void test_expr_gen(const std::vector<AST::DeclarationVariant>& sts)
    {
        for (auto & d : sts)
        {
            generate(d);
        }

        mod->print(llvm::outs(), nullptr);
        // check if its generating good IR
        if (llvm::verifyModule(*mod, &llvm::errs()))
        {
            llvm::errs() << "Module verification failed!\n";
        }
    }   

    void generate(const AST::DeclarationVariant& d)
    {
        std::visit([&](auto& x)
        {
            dgen(x);
        }, d);
    }

    llvm::Instruction* generate(const AST::StatementVariant& s)
    {
        static llvm::Instruction* last_visited = nullptr;
        std::visit([&](auto& x) { last_visited = this->sgen(x); }, s);
        return last_visited;
    }

    // useful visitor impl
    llvm::Value* generate(const AST::ExprVariant& e) 
    {
        static llvm::Value* last_visited; 
        std::visit([&](auto& x) { last_visited = this->gen(x); }, e);
        return last_visited;
    }

    auto printf_decl() const
    {
        // 0 = default address space (this might be used for other things like gpu memory for example)
        static llvm::FunctionType* prototype = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context),
            llvm::PointerType::get(llvm::Type::getInt8Ty(*context), 0), true);
        static llvm::Function* printf_func = llvm::Function::Create(
            prototype,
            llvm::Function::ExternalLinkage, // external function
            "printf",
            *mod
        );
        static llvm::FunctionCallee printfCallee = mod->getOrInsertFunction(
            "printf", prototype
        );
        return printfCallee;
    }

    //declaration generators 
    llvm::Function* dgen(const std::unique_ptr<AST::FunctionDeclaration>& fd);

    // statement generators
    llvm::Instruction* sgen(const std::unique_ptr<AST::ReturnStatement>& r);
    llvm::Instruction* sgen(const std::unique_ptr<AST::BlockStatement>& block);
    llvm::Instruction* sgen(const std::unique_ptr<AST::PrintStatement>& s);
    llvm::Instruction* sgen(const std::unique_ptr<AST::VariableDecl>& a);
    llvm::Instruction* sgen(const std::unique_ptr<AST::ExpressionStatement>& e);
    llvm::Instruction* sgen(const std::unique_ptr<AST::IfElseStatement>& e);
    llvm::Instruction* sgen(const std::unique_ptr<AST::WhileStatement>& e);
    
    llvm::Value* gen(const AST::Literal& lit);
    llvm::Value* gen(const AST::Variable& var);

    llvm::Value* gen(const std::unique_ptr<AST::Binary>& bin);
    llvm::Value* generate_unary_int_ops(const std::unique_ptr<AST::Unary>& un);
    llvm::Value* generate_unary_double_ops(const std::unique_ptr<AST::Unary>& un);
    llvm::Value* gen(const std::unique_ptr<AST::Unary>& un);
    llvm::Value* gen(const std::unique_ptr<AST::Assignment>& asn);
    llvm::Value* gen(const std::unique_ptr<AST::Call>& call);
    llvm::Value* gen(const std::unique_ptr<AST::StructAccess>& sa);
    llvm::Value* gen(const std::unique_ptr<AST::ArrayAccess>& aa);

    llvm::Value* generate_int_ops(const std::unique_ptr<AST::Binary>& bin);
    llvm::Value* generate_precise_ops(const std::unique_ptr<AST::Binary>& bin);

private:
    // for variables
    bool value_flag = true; 

    // stores internal info that we just pass around
    std::unique_ptr<llvm::LLVMContext> context; 
    // creates ir instructions
    std::unique_ptr<llvm::IRBuilder<>> builder; 
    // the "translation unit"
    std::unique_ptr<llvm::Module> mod; 
    // llvm types for creating variables
    std::unordered_map<std::string, llvm::Type*> type_to_llvm_ty;
    // store variables that exist
    // TODO: support the whole stack frames thing
    std::unordered_map<std::string, llvm::AllocaInst*> variable_locations;
};


#endif // COMPILER_H