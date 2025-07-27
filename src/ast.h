#ifndef AST_H
#define AST_H

#include <variant>
#include <memory>
#include <vector>
#include <string>
#include "lexer.h"

namespace AST {

    using LiteralVariant = std::variant<int, std::string, bool, char>; 

    struct Expression;
    struct Equality; 
    struct Literal;
    struct Binary;
    struct Unary; 
    struct Assignment; 
    struct Call;  
    struct Variable; 
    struct StructAccess;
    struct ArrayAccess;

    struct Expression
    {
        std::size_t line; 
        virtual ~Expression() = default;
    };

    struct Variable : Expression 
    {
        Token name;
    }; 

    struct Literal : Expression 
    {
        LiteralVariant value;
    }; 

    template<typename T> using _up = std::unique_ptr<T>;

    using ExprVariant = std::variant<
        _up<Equality>,
        _up<Unary>,
        Literal,
        _up<Binary>,
        _up<Assignment>,
        _up<Call>,
        Variable,
        _up<StructAccess>,
        _up<ArrayAccess>
    >;

    struct Binary : Expression
    {
        ExprVariant left; 
        ExprVariant right;
        TokenType op; 
    };

    struct Unary : Expression
    {
        ExprVariant operand; 
        TokenType op;
    }; 

    struct Assignment : Expression
    {
        ExprVariant lhs; 
        ExprVariant rhs; 
        TokenType op; 
    };

    struct Call : Expression
    {
        Token func_name;
        std::vector<ExprVariant> args; 
    };

    struct StructAccess : Expression 
    {
        ExprVariant lhs; 
        std::string member_name; 
    }; 

    struct ArrayAccess : Expression 
    {
        ExprVariant lhs; 
        ExprVariant index; 
    };

    struct ExprVisitor
    {
        virtual void operator()(_up<Equality>&) = 0;
        virtual void operator()(_up<Unary>&) = 0;
        virtual void operator()(const Literal&) = 0;
        virtual void operator()(_up<Binary>&) = 0;
        virtual void operator()(_up<Assignment>&) = 0;
        virtual void operator()(_up<Call>&) = 0;
        virtual void operator()(const Variable&) = 0;
        virtual void operator()(_up<StructAccess>&) = 0;
        virtual void operator()(_up<ArrayAccess>&) = 0;
    };

} // namespace AST

#endif // AST_H
