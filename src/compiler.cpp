#include "compiler.h"

llvm::Value *Codegen::gen_literal(const AST::Literal &lit)
{
    auto generate_ir = [&](auto&& literal) -> llvm::Value* 
    {
        using T = std::decay_t<decltype(literal)>; // get the underlying type

        if (std::is_same_v<T, float>)
        {
            return ConstantFP::
        }
        


        return nullptr; 
    };


    return std::visit(generate_ir, lit.value);
}

llvm::Value *Codegen::gen_variable(const AST::Variable &var)
{
    return nullptr;
}

llvm::Value *Codegen::gen_binary(const AST::Binary &bin)
{
    return nullptr;
}

llvm::Value *Codegen::gen_unary(const AST::Unary &un)
{
    return nullptr;
}

llvm::Value *Codegen::gen_assignment(const AST::Assignment &asn)
{
    return nullptr;
}

llvm::Value *Codegen::gen_call(const AST::Call &call)
{
    return nullptr;
}

llvm::Value *Codegen::gen_struct_access(const AST::StructAccess &sa)
{
    return nullptr;
}

llvm::Value *Codegen::gen_array_access(const AST::ArrayAccess &aa)
{
    return nullptr;
}
