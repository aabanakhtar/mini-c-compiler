#include "parser.h"
#include <iostream>
#include <string>

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

void TreePrinter::operator()(const AST::Literal& lit)
{
    indent();
    std::visit([](auto&& val) {
        std::cout << "Literal: " << val << "\n";
    }, lit.value);
}

AST::ExprVariant Parser::get_program()
{
    return parse_assignment(); 
}

AST::ExprVariant Parser::parse_assignment()
{
    auto lhs = parse_logic_or(); 

    while (check(TokenType::EQUAL)) 
    {
        auto op = advance(); // take in the equals 
        auto rhs = parse_logic_or();
        lhs = std::make_unique<AST::Assignment>(get_line(lhs), std::move(lhs), op, std::move(rhs)); 
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

// tbd 