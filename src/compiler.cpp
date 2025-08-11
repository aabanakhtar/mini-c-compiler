#include "compiler.h"

#include <iostream>

Codegen::Codegen()
{
    context = std::make_unique<llvm::LLVMContext>();
    mod = std::make_unique<llvm::Module>("main", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

llvm::Value *Codegen::gen(const AST::Literal &lit)
{
    auto generate_ir = [&]<typename T0>(T0&& literal) -> llvm::Value*
    {
        using T = std::decay_t<T0>; // get the underlying type

        if (std::is_same_v<T, float>)
        {
            // get the type if it exists or create it in the context
            auto type = llvm::Type::getFloatTy(*context); 
            // generate it
            return llvm::ConstantFP::get(type, literal); 
        } 
        else if (std::is_same_v<T, double>)
        {
            auto type = llvm::Type::getDoubleTy(*context); 
            return llvm::ConstantFP::get(type, literal); // doobles work with constant FP
        }
        else if (std::is_same_v<T, int>)
        {
            auto type = llvm::Type::getInt32Ty(*context); 
            return llvm::ConstantInt::get(type, literal, true); // true = signed
        } 
        else if (std::is_same_v<T, char>)
        {
            auto type = llvm::Type::getInt8Ty(*context); 
            return llvm::ConstantInt::get(type, literal, true);
        }

        return nullptr; 
    };


    return std::visit(generate_ir, lit.value);
}


llvm::Value* Codegen::gen(const AST::Variable& var)
{
    return nullptr;
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Binary>& bin)
{
    switch (AST::get_type(bin->left)) //  assumes semantic analyzer returned a valid ast (TODO:)
    {
    case AST::LiteralType::INT:
        return generate_int_ops(bin);
    case AST::LiteralType::FLOAT:
        return generate_precise_ops(bin);
    case AST::LiteralType::DOUBLE:
        return generate_precise_ops(bin);
    case AST::LiteralType::CHAR:
        return generate_int_ops(bin);
    default:
        return nullptr;
    }
}

llvm::Value* Codegen::generate_unary_int_ops(const std::unique_ptr<AST::Unary>& un)
{
    const auto rhs = generate(un->operand);

    switch (un->op)
    {
    case TokenType::MINUS:
    {
        const auto type = llvm::Type::getInt32Ty(*context);
        const auto zero_val = llvm::ConstantInt::get(type, 0, true);
        return builder->CreateSub(zero_val, rhs);
    }
    case TokenType::PLUS:
        return rhs; // unary + has no effect on the operand
    default:
        return nullptr;
    }
}

llvm::Value* Codegen::generate_unary_double_ops(const std::unique_ptr<AST::Unary>& un)
{
    const auto rhs = generate(un->operand);

    switch (un->op)
    {
    case TokenType::MINUS:
    {
        const auto type = llvm::Type::getDoubleTy(*context);
        const auto zero_val = llvm::ConstantFP::get(type, 0.0);
        return builder->CreateFSub(zero_val, rhs);
    }
    case TokenType::PLUS:
        return rhs; // unary + has no effect on the operand
    default:
        return nullptr;
    }
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Unary>& un)
{
    switch (get_type(un->operand))
    {
    case AST::LiteralType::INT:
        return generate_unary_int_ops(un);
    case AST::LiteralType::DOUBLE:
        return generate_unary_double_ops(un);
    default: return nullptr;
    }
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Assignment>& asn)
{
    return nullptr;
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Call>& call)
{
    return nullptr;
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::StructAccess>& sa)
{
    return nullptr;
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::ArrayAccess>& aa)
{
    return nullptr;
}

llvm::Value* Codegen::generate_int_ops(const std::unique_ptr<AST::Binary>& bin)
{
    const auto left = generate(bin->left);
    const auto right = generate(bin->right);

    if (!left || !right)
    {
        return nullptr;
    }

    switch (bin->op)
    {
    case TokenType::PLUS:
        return builder->CreateAdd(left, right);
    case TokenType::MINUS:
        return builder->CreateSub(left, right);
    case TokenType::STAR:
        return builder->CreateMul(left, right);
    case TokenType::SLASH:
        return builder->CreateSDiv(left, right);
    case TokenType::EQUAL_EQUAL:
        return builder->CreateICmpEQ(left, right); // integer cmp eq
    case TokenType::BANG_EQUAL:
        return builder->CreateICmpNE(left, right); // integer cmp !eq what are these names vro
    case TokenType::AND:
    case TokenType::OR:
    default:
        return nullptr; // shouldn't reach here unless something is weird
    }
}

llvm::Value* Codegen::generate_precise_ops(const std::unique_ptr<AST::Binary>& bin)
{
    const auto left = generate(bin->left);
    const auto right = generate(bin->right);

    if (!left || !right)
    {
        return nullptr;
    }

    switch (bin->op)
    {
    case TokenType::PLUS:
        return builder->CreateFAdd(left, right);
    case TokenType::MINUS:
        return builder->CreateFSub(left, right);
    case TokenType::STAR:
        return builder->CreateFMul(left, right);
    case TokenType::SLASH:
        return builder->CreateFDiv(left, right);
    // c is ordered, meaning that NaN is treated as a bogus value and is never equal to anything
    case TokenType::EQUAL_EQUAL:
        return builder->CreateFCmpOEQ(left, right); // integer cmp eq
    case TokenType::BANG_EQUAL:
        return builder->CreateFCmpONE(left, right); // integer cmp !eq what are these names vro
    case TokenType::AND:
    case TokenType::OR: 
    default:
        return nullptr; // shouldn't reach here unless something is weird
    }
}
