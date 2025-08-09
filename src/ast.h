#ifndef AST_H
#define AST_H

#include <variant>
#include <memory>
#include <vector>
#include <string>
#include "lexer.h"

namespace AST {

    using LiteralVariant = std::variant<int, std::string, char, float, double>; 

    struct Expression;
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
        Expression(size_t line) : line(line) {} 
        virtual ~Expression() = default;
    };

    struct Variable : Expression 
    {
        Token name;

        Variable(size_t line, Token name) : Expression(line), name(std::move(name)) {}
    }; 

    struct Literal : Expression 
    {
        LiteralVariant value;

        Literal(size_t line, LiteralVariant value) : Expression(line), value(value) {}
    }; 

    template<typename T> using _up = std::unique_ptr<T>;

    using ExprVariant = std::variant<
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

        Binary(std::size_t line, ExprVariant left, TokenType op, ExprVariant right)
            : Expression(line), left(std::move(left)), right(std::move(right)), op(op)
        {}
    };

    struct Unary : Expression
    {
        ExprVariant operand;
        TokenType op;

        Unary(std::size_t line, TokenType op, ExprVariant operand)
            : Expression(line), operand(std::move(operand)), op(op)
        {}
    };

    struct Assignment : Expression
    {
        ExprVariant lhs;
        ExprVariant rhs;
        TokenType op;

        Assignment(std::size_t line, ExprVariant lhs, TokenType op, ExprVariant rhs)
            : Expression(line), lhs(std::move(lhs)), rhs(std::move(rhs)), op(op)
        {}
    };

    struct Call : Expression
    {
        Token func_name;
        std::vector<ExprVariant> args;

        Call(std::size_t line, Token func_name, std::vector<ExprVariant> args)
            : Expression(line), func_name(std::move(func_name)), args(std::move(args))
        {}
    };

    struct StructAccess : Expression
    {
        ExprVariant lhs;
        std::string member_name;

        StructAccess(std::size_t line, ExprVariant lhs, std::string member_name)
            : Expression(line), lhs(std::move(lhs)), member_name(std::move(member_name))
        {}
    };

    struct ArrayAccess : Expression 
    {
        ExprVariant lhs; 
        ExprVariant index; 

        ArrayAccess(size_t line, ExprVariant lhs, ExprVariant index) : 
            Expression(line), lhs(std::move(lhs)), index(std::move(index))
        {}
    };

    struct ExprVisitor
    {
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
