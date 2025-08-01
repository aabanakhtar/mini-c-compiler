#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "util.h"

struct TreePrinter : AST::ExprVisitor
{
    size_t indent_level = 0; 
    void indent();

    virtual void operator()(std::unique_ptr<AST::Binary>&) override;
    virtual void operator()(const AST::Literal&) override;
    virtual void operator()(std::unique_ptr<AST::Unary>&) override;
    virtual void operator()(std::unique_ptr<AST::Assignment>&) override;
    virtual void operator()(std::unique_ptr<AST::Call>&) override;
    virtual void operator()(const AST::Variable&) override;
    virtual void operator()(std::unique_ptr<AST::StructAccess>&) override;
    virtual void operator()(std::unique_ptr<AST::ArrayAccess>&) override;
};

class Parser 
{
public: 
    AST::ExprVariant get_program(); 
    AST::ExprVariant parse_assignment();
    AST::ExprVariant parse_logic_or();   
    AST::ExprVariant parse_logic_and();
    AST::ExprVariant parse_equality();
    AST::ExprVariant parse_relational();
    AST::ExprVariant parse_additive();
    AST::ExprVariant parse_multiplicative();
    AST::ExprVariant parse_unary();
    AST::ExprVariant parse_postfix();        // for call, array, struct access: (), [], .
    AST::ExprVariant parse_primary();        // for literals, identifiers, grouped expressions

    void panic();
    bool check(TokenType t);
    bool check(Tokentype* t, size_t len, TokenType& found);
    TokenType advance();   
private: 
    std::vector<Token> tokens;
    size_t current_tok = 0;

    template<typename Variant>
    inline size_t get_line(const Variant& node)
    {
        return std::visit([](const auto& value) -> size_t {
            using T = std::decay_t<decltype(value)>; // get the base tyhpe
            if constexpr (std::is_same_v<T, AST::Literal> || std::is_same_v<T, AST::Variable>)
            {
                return value.line;
            }
            else
            {
                return value->line;
            }
        }, node);
    }
};

#endif // PARSER_H