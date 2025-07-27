#ifndef AST_H
#define AST_H

#include <variant>
#include <memory>
#include "lexer.h"

namespace AST {
    struct Expression; 
    struct Equality; 
    struct Literal;
    struct Unary; 
    struct Assignment; 
    struct Call;  
    struct Variable; 
    struct StructAccess;
    struct ArrayAccess;

    using LiteralVariant = std::variant<int, std::string, bool, char>; 
    using ExprVariant = std::variant<
        std::unique_ptr<Expression>,
        std::unique_ptr<Equality>,
        std::unique_ptr<Unary>,
        Literal,
        std::unique_ptr<Assignment>,
        std::unique_ptr<Call>,
        Variable,
        std::unique_ptr<StructAccess>,
        std::unique_ptr<ArrayAccess>
    >;

    struct Expression
    {
        std::size_t line; 
    };

    struct Binary : Expression
    {
        ExprVariant left; 
        ExprVariant right;
        TokenType op; 
    };

    struct Literal : Expression 
    {
        LiteralVariant value; 
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

    struct Variable : Expression 
    {
        Token name; 
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
}



#endif // AST_H