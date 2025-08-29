#ifndef AST_H
#define AST_H

#include <variant>
#include <memory>
#include <vector>
#include <string>
#include "lexer.h"

namespace AST
{
    enum class LiteralType
    {
        INT, CHAR, FLOAT, DOUBLE, CSTRING, UNKNOWN
    };

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

        explicit Expression(const size_t line) : line(line)
        {
        }

        virtual ~Expression() = default;

        std::string result_type = "__13190381minic_unknown"; // initialized in semantic analysis
    };

    struct Variable : Expression
    {
        Token name;

        Variable(const size_t line, Token name) : Expression(line), name(std::move(name))
        {
        }
    };

    struct Literal : Expression
    {
        LiteralVariant value;

        Literal(const size_t line, LiteralVariant value) : Expression(line), value(std::move(value))
        {
        }

        Literal(const Literal &other)
            : Expression(other.line), value(other.value)
        {
            result_type = other.result_type;
        }

        Literal& operator=(const Literal& other)
        {
            if (this != &other)
            {
                line = other.line;
                value = other.value;
                result_type = other.result_type;
            }
            return *this;
        }
    };

    template <typename T>
    using _up = std::unique_ptr<T>;

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

        Binary(const std::size_t line, ExprVariant left, TokenType op, ExprVariant right)
            : Expression(line), left(std::move(left)), right(std::move(right)), op(op)
        {
        }
    };

    struct Unary : Expression
    {
        ExprVariant operand;
        TokenType op;

        Unary(const std::size_t line, const TokenType op, ExprVariant operand)
            : Expression(line), operand(std::move(operand)), op(op)
        {
        }
    };

    struct Assignment : Expression
    {
        ExprVariant lhs;
        ExprVariant rhs;
        TokenType op;

        Assignment(const std::size_t line, ExprVariant lhs, const TokenType op, ExprVariant rhs)
            : Expression(line), lhs(std::move(lhs)), rhs(std::move(rhs)), op(op)
        {
        }
    };

    struct Call : Expression
    {
        Token func_name;
        std::vector<ExprVariant> args;

        Call(const std::size_t line, Token func_name, std::vector<ExprVariant> args)
            : Expression(line), func_name(std::move(func_name)), args(std::move(args))
        {
        }
    };

    struct StructAccess : Expression
    {
        ExprVariant lhs;
        std::string member_name;

        StructAccess(const std::size_t line, ExprVariant lhs, std::string member_name)
            : Expression(line), lhs(std::move(lhs)), member_name(std::move(member_name))
        {
        }
    };

    struct ArrayAccess : Expression
    {
        ExprVariant lhs;
        ExprVariant index;

        ArrayAccess(size_t line, ExprVariant lhs, ExprVariant index) :
            Expression(line), lhs(std::move(lhs)), index(std::move(index))
        {
        }
    };

    struct ExprVisitor
    {
        virtual ~ExprVisitor() = default;
        virtual void operator()(_up<Unary>&) = 0;
        virtual void operator()(const Literal&) = 0;
        virtual void operator()(_up<Binary>&) = 0;
        virtual void operator()(_up<Assignment>&) = 0;
        virtual void operator()(_up<Call>&) = 0;
        virtual void operator()(const Variable&) = 0;
        virtual void operator()(_up<StructAccess>&) = 0;
        virtual void operator()(_up<ArrayAccess>&) = 0;
    };

    inline std::string get_literal_type(const AST::Literal& lit)
    {
        auto get_type_enum = [&]<typename T0>(T0&& x)
        {
            using U = std::decay_t<T0>;
            if constexpr (std::is_same_v<U, char>)
            {
                return "char";
            }
            else if constexpr (std::is_same_v<U, int>)
            {
                return "int";
            }
            else if constexpr (std::is_same_v<U, float>)
            {
                return "float";
            }
            else if constexpr (std::is_same_v<U, double>)
            {
                return "double";
            }
            else if constexpr (std::is_same_v<U, std::string>)
            {
                return "const char*";
            }

            return "__19192838mini_c_unknown"; // im slick with it
        };

        return std::visit(get_type_enum, lit.value);
    }

    // using stdlib is such a pain 😭
    inline std::string get_type(const ExprVariant& e)
    {
        return std::visit([&]<typename T0>(T0& x)
        {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, Literal> || std::is_same_v<T, Variable>)
            {
                return x.result_type;
            }
            else
            {
                return x->result_type;
            }
        }, e);
    }

    struct Statement
    {
        std::size_t line;

        explicit Statement(const size_t line) : line(line)
        {
        }

        virtual ~Statement() = default;
    };

    struct PrintStatement;
    struct VariableDecl;
    struct ExpressionStatement;
    struct IfElseStatement;
    struct WhileStatement;

    using StatementVariant = std::variant<
        _up<PrintStatement>,
        _up<VariableDecl>,
        _up<ExpressionStatement>,
        _up<IfElseStatement>,
        _up<WhileStatement>
    >;

    struct PrintStatement : Statement
    {
        AST::ExprVariant value;

        explicit PrintStatement(const std::size_t line, AST::ExprVariant value)
            : Statement(line), value(std::move(value))
        {
        }
    };

    struct VariableDecl : Statement
    {
        std::string name;
        std::string type;
        ExprVariant value;

        VariableDecl(const size_t line, const std::string& name, const std::string& type, ExprVariant& value)
             : Statement(line),
               name(name),
               type(type),
               value(std::move(value))
        {
        }

    };

    struct ExpressionStatement : Statement
    {
        ExprVariant expr;

        ExpressionStatement(const std::size_t line, ExprVariant& value)
            : Statement(line),
              expr(std::move(value))
        {
        }
    };

    struct IfElseStatement : Statement
    {
        ExprVariant condition;
        StatementVariant if_body;
        StatementVariant else_body;

        IfElseStatement(const std::size_t line, ExprVariant& condition, StatementVariant& if_body, StatementVariant& else_body) :
            Statement(line), condition(std::move(condition)), if_body(std::move(if_body)), else_body(std::move(else_body))
        {
        }
    };

    struct WhileStatement : Statement 
    {
        ExprVariant condition;
        StatementVariant body;

        WhileStatement(const std::size_t line, ExprVariant& condition, StatementVariant& body) :
            Statement(line), condition(std::move(condition)), body(std::move(body))
        {
        }
    };

    inline std::string literal_type_to_str(const LiteralType literal)
    {
        switch (literal)
        {
        case LiteralType::CHAR: return "char";
        case LiteralType::INT: return "int";
        case LiteralType::FLOAT: return "float";
        case LiteralType::DOUBLE: return "double";
        case LiteralType::CSTRING: return "const char*";
        case LiteralType::UNKNOWN: return "unknown";
        }

        return "";
    }
} // namespace AST

#endif // AST_H
