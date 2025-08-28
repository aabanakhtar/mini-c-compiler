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
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens)
    {
        int a = 3;
    }
    using Program = std::vector<AST::StatementVariant>;

    // statements
    Program get_program();
    AST::StatementVariant parse_statement();
    AST::StatementVariant parse_printf();
    AST::StatementVariant parse_variable_declaration();
    AST::StatementVariant parse_expression_statement();
    AST::StatementVariant parse_if_else_statement();
    AST::StatementVariant parse_while_statement(); 
    //AST::ExprVariant get_program();
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

    void panic(const std::string& why, std::size_t line) const;
    bool check(TokenType t) const;
    const Token& expect(TokenType t, const std::string& error);
    // checks if the token can represent the start of a initialization for a var
    bool is_type(const Token& tok);
    // check for various types of tokens in an array of any kind
    bool check(TokenType* t, std::size_t len, TokenType& found) const;
    const Token& advance();
    Token peek() const { return tokens[current_tok < tokens.size() ? current_tok : tokens.size() - 1]; }
private:
    const std::vector<Token>& tokens;
    size_t current_tok = 0;
    bool is_panic = false;

    template<typename Variant>
    inline size_t get_line(const Variant& node)
    {
        return std::visit([]<typename T0>(const T0& value) -> size_t {
            using T = std::decay_t<T0>; // get the base type
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