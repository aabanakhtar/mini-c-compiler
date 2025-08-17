#include "parser.h"
#include <iostream>
#include <string>
#include <sstream>
#include "error.h"

void TreePrinter::indent()
{
    for (std::size_t i = 0; i < indent_level; ++i)
    {
        std::cout << "  ";
    }
}

void TreePrinter::operator()(std::unique_ptr<AST::Binary>& bin)
{
    indent(); std::cout << "BinaryExpression: op = " << static_cast<int>(bin->op) << "\n";
    indent_level++;
    indent(); std::cout << "Left:\n";
    indent_level++;
    std::visit(*this, bin->left);
    indent_level--;
    indent(); std::cout << "Right:\n";
    indent_level++;
    std::visit(*this, bin->right);
    indent_level -= 2;
}

void TreePrinter::operator()(std::unique_ptr<AST::Unary>& unary)
{
    indent(); std::cout << "UnaryExpression: op = " << static_cast<int>(unary->op) << "\n";
    indent_level++;
    indent(); std::cout << "Operand:\n";
    indent_level++;
    std::visit(*this, unary->operand);
    indent_level -= 2;
}

void TreePrinter::operator()(std::unique_ptr<AST::Assignment>& assign)
{
    indent(); std::cout << "Assignment: op = " << static_cast<int>(assign->op) << "\n";
    indent_level++;
    indent(); std::cout << "LHS:\n";
    indent_level++;
    std::visit(*this, assign->lhs);
    indent_level--;
    indent(); std::cout << "RHS:\n";
    indent_level++;
    std::visit(*this, assign->rhs);
    indent_level -= 2;
}

void TreePrinter::operator()(std::unique_ptr<AST::Call>& call)
{
    indent(); std::cout << "FunctionCall: " << call->func_name.value << "\n";
    indent_level++;
    indent(); std::cout << "Arguments:\n";
    indent_level++;
    for (auto& arg : call->args)
    {
        std::visit(*this, arg);
    }
    indent_level -= 2;
}

void TreePrinter::operator()(const AST::Variable& var)
{
    indent(); std::cout << "Variable: " << var.name.value << "\n";
}

void TreePrinter::operator()(std::unique_ptr<AST::StructAccess>& sa)
{
    indent(); std::cout << "StructAccess: ." << sa->member_name << "\n";
    indent_level++;
    indent(); std::cout << "Object:\n";
    indent_level++;
    std::visit(*this, sa->lhs);
    indent_level -= 2;
}

void TreePrinter::operator()(std::unique_ptr<AST::ArrayAccess>& aa)
{
    indent(); std::cout << "ArrayAccess:\n";
    indent_level++;
    indent(); std::cout << "Array:\n";
    indent_level++;
    std::visit(*this, aa->lhs);
    indent_level--;
    indent(); std::cout << "Index:\n";
    indent_level++;
    std::visit(*this, aa->index);
    indent_level -= 2;
}

Parser::Program Parser::get_program()
{
    auto p = Program{};
    p.push_back(parse_printf());
    return p;
}

void Parser::expect(const TokenType t, const std::string& error)
{
    if (!check(t))
    {
        advance();
        report_err(std::cout, error);
        is_panic = true;
    }

    advance();
}

AST::StatementVariant Parser::parse_statement()
{
    return parse_printf();
}

AST::StatementVariant Parser::parse_printf()
{
    std::size_t line = advance().line; // printf
    expect(TokenType::LEFT_PAREN, "Expected '(' after printf");
    auto expr = parse_primary();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after printf.");
    expect(TokenType::SEMICOLON, "Expected ';' after printf.");

    auto print = std::make_unique<AST::PrintStatement>(line, std::move(expr));
    return print;
}

void TreePrinter::operator()(const AST::Literal& lit)
{
    indent();
    std::visit([](auto&& val) {
        std::cout << "Literal: " << val << "\n";
    }, lit.value);
}

/*
AST::ExprVariant Parser::get_program()
{
    return parse_assignment(); 
}
*/

AST::ExprVariant Parser::parse_assignment()
{
    auto lhs = parse_logic_or(); 

    while (check(TokenType::EQUAL)) 
    {
        auto op = advance(); // take in the equals 
        auto rhs = parse_logic_or();
        lhs = std::make_unique<AST::Assignment>(get_line(lhs), std::move(lhs), op.type, std::move(rhs)); 
    }

    return lhs; 
}

AST::ExprVariant Parser::parse_logic_or() 
{
    auto lhs = parse_logic_and(); 
    while (check(TokenType::OR)) 
    {
        advance(); // get rid of ampersand
        auto rhs = parse_logic_and(); 
        lhs = std::make_unique<AST::Binary>(get_line(lhs), std::move(lhs), TokenType::OR, std::move(rhs)); 
    }

    return lhs; 
}

AST::ExprVariant Parser::parse_logic_and()
{
    auto lhs = parse_equality(); 
    while (check(TokenType::OR)) 
    {
        advance(); // get rid of ampersand
        auto rhs = parse_equality(); 
        lhs = std::make_unique<AST::Binary>(get_line(lhs), std::move(lhs), TokenType::AND, std::move(rhs)); 
    }

    return lhs; 
}

AST::ExprVariant Parser::parse_equality()
{
    static TokenType equality_ops[] = {TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL};
    
    TokenType found_token;
    auto lhs = parse_relational(); 
    while (check(equality_ops, sizeof(equality_ops) / sizeof(TokenType), found_token)) 
    {
        advance(); // get rid of ampersand
        auto rhs = parse_relational(); 
        lhs = std::make_unique<AST::Binary>(get_line(lhs), std::move(lhs), found_token, std::move(rhs)); 
    }

    return lhs; 
}

AST::ExprVariant Parser::parse_relational()
{
    static TokenType relational_ops[] = {TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL};
    
    TokenType found_token;
    auto lhs = parse_additive(); 
    while (check(relational_ops, sizeof(relational_ops) / sizeof(TokenType), found_token)) 
    {
        advance(); // get rid of ampersand
        auto rhs = parse_additive(); 
        lhs = std::make_unique<AST::Binary>(get_line(lhs), std::move(lhs), found_token, std::move(rhs)); 
    }

    return lhs; 
}

AST::ExprVariant Parser::parse_additive()
{
    static TokenType additive_ops[] = {TokenType::PLUS, TokenType::MINUS}; 

    TokenType found_token; 
    auto lhs = parse_multiplicative(); 
    while (check(additive_ops, sizeof(additive_ops) / sizeof(TokenType), found_token))
    {
        advance(); 
        auto rhs = parse_multiplicative(); 
        lhs = std::make_unique<AST::Binary>(get_line(lhs), std::move(lhs), found_token, std::move(rhs)); 
    }

    return lhs; 
}

AST::ExprVariant Parser::parse_multiplicative()
{
    static TokenType mul_ops[] = {TokenType::STAR, TokenType::SLASH}; 

    TokenType found_token; 
    auto lhs = parse_unary(); 
    while (check(mul_ops, sizeof(mul_ops) / sizeof(TokenType), found_token))
    {
        advance(); 
        auto rhs = parse_unary(); 
        lhs = std::make_unique<AST::Binary>(get_line(lhs), std::move(lhs), found_token, std::move(rhs)); 
    }

    return lhs; 
}

AST::ExprVariant Parser::parse_unary()
{
    static TokenType unary_ops[] = {TokenType::MINUS, TokenType::STAR, TokenType::PLUS};

    if (TokenType found_token; check(unary_ops, sizeof(unary_ops) / sizeof(TokenType), found_token))
    {
        auto line = advance().line; 
        return std::make_unique<AST::Unary>(line, found_token, parse_unary()); 
    }

    return parse_postfix(); 
}

// TODO: update when finding postfix stuff
AST::ExprVariant Parser::parse_postfix()
{
    return parse_primary();
}

AST::ExprVariant Parser::parse_primary()
{
    if (check(TokenType::NUMBER) || check(TokenType::STRING))
    {
        const auto val = advance();
        AST::Literal literal(0, 0);

        if (val.type == TokenType::STRING)
        {
            literal = AST::Literal(val.line, val.value);
        }
        else
        {
            literal = AST::Literal(val.line, std::stoi(val.value));
        }

        return literal;
    }
    else
    {
        const auto offender = advance();  // get the offending token
        panic("Failed to parse expression!", offender.line);
        return AST::Literal(0, 0);
    }
}

void Parser::panic(const std::string& why, const std::size_t line) const
{
    std::ostringstream ss; 
    ss << "FAILED PARSE! " << why << " on line: " << line << ".\n";
    report_err(std::cout, ss.str());
}


bool Parser::check(const TokenType t) const
{
    if (tokens[current_tok].type == t) 
    {
        return true;
    }

    return false; 
}


bool Parser::check(TokenType* types, std::size_t len, TokenType& found)
{
    for (std::size_t i = 0; i < len; ++i)
    {
        if (tokens[current_tok].type == types[i]) 
        {
            found = tokens[current_tok].type;
            return true;
        }
    }

    return false; 
}


Token Parser::advance()
{
    if (current_tok + 1 < tokens.size() - 1)
    {
        return tokens[current_tok++]; 
    }
    
    return tokens[tokens.size() - 1];
}
