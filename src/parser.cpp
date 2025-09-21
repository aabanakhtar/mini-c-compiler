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
    while (current_tok < tokens.size() - 1)
    {
        p.push_back(parse_function_declaration());
        if (is_panic)
        {
            // just stop parsing, and keep life simple; this is not production code anyway
            break;
        }
    }
    return p;
}

AST::DeclarationVariant Parser::parse_function_declaration()
{
    // get the main header info
    auto return_type_token = advance(); 
    auto line = return_type_token.line; 
    auto return_ty = return_type_token.value;
    auto name_token = expect(TokenType::IDENTIFIER, "Expected function name after return type in function declaration.").value;
    expect(TokenType::LEFT_PAREN, "Expected ( after function name in function declaration.");
    // parse arguments
    std::vector<AST::FunctionDeclaration::FunctionArg> params;
    if (!check(TokenType::RIGHT_PAREN))
    {
        // get the arg first, then loop back for the next one if possible
        do 
        {
            auto type_token = advance(); 
            auto type = type_token.value; 
            if (type_token.type == TokenType::STRUCT)
            {
                type = expect(TokenType::IDENTIFIER, "Expected struct name after 'struct' keyword in function argument.").value;
            }
            auto name = expect(TokenType::IDENTIFIER, "Expected argument name after type in function argument.").value;
            params.emplace_back(type, name);
        } 
        while (check(TokenType::COMMA) && advance().type == TokenType::COMMA);
    }
    // close off args
    expect(TokenType::RIGHT_PAREN, "Expected ) after function arguments in function declaration.");
    // get the body
    auto body_variant = parse_block_statement();
    // Extract BlockStatement from variant
    auto body_ptr = std::move(std::get<std::unique_ptr<AST::BlockStatement>>(body_variant));
    return std::make_unique<AST::FunctionDeclaration>(line, name_token, return_ty, params, std::move(body_ptr));
}

AST::StatementVariant Parser::parse_block_statement()
{
    auto statements = std::vector<AST::StatementVariant>{};
    std::size_t line = advance().line; // advance past the {

    statements.reserve(20);
    while (peek().type != TokenType::RIGHT_BRACE && !is_panic)
    {
        statements.push_back(parse_statement());
    }

    if (!is_panic) expect(TokenType::RIGHT_BRACE, "Expected } to close off block statement");

    return std::make_unique<AST::BlockStatement>(line, statements);
}

AST::StatementVariant Parser::parse_return_statement()
{
    auto line = expect(TokenType::RETURN, "Expected return keyword for return statement!").line;
    // handle void returns
    if (peek().type == TokenType::SEMICOLON)
    {
        advance(); // get rid of the semicolon
        return std::make_unique<AST::ReturnStatement>(line, std::nullopt);
    }

    auto expr = parse_assignment();
    expect(TokenType::SEMICOLON, "Expected ; after return expression!");
    return std::make_unique<AST::ReturnStatement>(line, std::make_optional(std::move(expr)));
}

const Token& Parser::expect(const TokenType t, const std::string& error)
{
    if (!check(t))
    {
        advance(); // get rid of this ting
        report_err(std::cout, error);
        is_panic = true;
    }

    return advance();
}

bool Parser::is_type(const Token& tok)
{
    switch (tok.type) {
    case TokenType::INT:
    case TokenType::VOID:
    case TokenType::CHAR:
        return true;
    // must have a ident after the struct keyword
    case TokenType::STRUCT:
        // look 2 ahead lol
        if (current_tok + 1 >= tokens.size()) return false;
        return tokens[current_tok + 1].type == TokenType::IDENTIFIER;
    default:
        return false;
    }
}

AST::StatementVariant Parser::parse_statement()
{
    if (is_type(peek()))
    {
        return parse_variable_declaration();
    }
    else if (peek().type == TokenType::IDENTIFIER && peek().value != "printf")
    {
        return parse_expression_statement();
    }
    else if (peek().type == TokenType::IF)
    {
        return parse_if_else_statement();
    }
    else if (peek().type == TokenType::WHILE)
    {
        return parse_while_statement();
    }
    else if (peek().type == TokenType::LEFT_BRACE)
    {
        return parse_block_statement();
    }
    else if (peek().type == TokenType::RETURN)
    {
        return parse_return_statement();
    }

    return parse_printf();
}

AST::StatementVariant Parser::parse_printf()
{
    std::size_t line = advance().line; // printf
    expect(TokenType::LEFT_PAREN, "Expected '(' after printf");
    auto expr = parse_assignment();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after printf.");
    expect(TokenType::SEMICOLON, "Expected ';' after printf.");
    auto print = std::make_unique<AST::PrintStatement>(line, std::move(expr));
    return print;
}

AST::StatementVariant Parser::parse_variable_declaration()
{
    const auto identifier = advance();
    auto type = identifier.value;
    // make sure to get the actual typename from the second one
    if (identifier.type == TokenType::STRUCT)
    {
        type = expect(TokenType::IDENTIFIER, "Expected an identifier after 'struct.'").value;
    }
    auto name = expect(TokenType::IDENTIFIER, "Expected a variable name.").value;
    expect(TokenType::EQUAL, "Expected '=' for assignment.");
    auto value = parse_assignment(); // top level expression from C standard
    expect(TokenType::SEMICOLON, "Expected ';' after assignment.");
    return std::make_unique<AST::VariableDecl>(identifier.line, name, type, value);
}

AST::StatementVariant Parser::parse_expression_statement()
{
    auto expr = parse_assignment(); // highest precedence
    expect(TokenType::SEMICOLON, "Expected ; after expression!");
    return std::make_unique<AST::ExpressionStatement>(get_line(expr), expr);
}

AST::StatementVariant Parser::parse_if_else_statement()
{
    auto line = expect(TokenType::IF, "Expected if for if statement!").line;
    expect(TokenType::LEFT_PAREN, "Expected ( to start conditional in if statement!");
    // get the internal condition
    auto condition = parse_assignment();
    expect(TokenType::RIGHT_PAREN, "Expected )");
    auto if_body = parse_statement();
    expect(TokenType::ELSE, "Expected else after if body."); // TODO: make this optional
    auto else_body = parse_statement();

    return std::make_unique<AST::IfElseStatement>(line, condition, if_body, else_body);
}

AST::StatementVariant Parser::parse_while_statement()
{
    auto line = expect(TokenType::WHILE, "Expected while for while statement!").line;
    expect(TokenType::LEFT_PAREN, "Expected ( to start conditional in while statement!");
    // get the internal condition
    auto condition = parse_assignment();
    expect(TokenType::RIGHT_PAREN, "Expected )");
    auto body = parse_statement();

    return std::make_unique<AST::WhileStatement>(line, condition, body);
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

    if (check(TokenType::EQUAL))
    {
        auto op = advance();
        auto rhs = parse_assignment();  // recursive call for right-associativity
        return std::make_unique<AST::Assignment>(get_line(lhs), std::move(lhs), op.type, std::move(rhs));
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
    // make sure that we dont accidentally parse a variable as a function call
    if (check(TokenType::IDENTIFIER) && peek().value != "printf" && current_tok + 1 < tokens.size() && tokens[current_tok + 1].type == TokenType::LEFT_PAREN)
    {
        auto id = advance(); 
        expect(TokenType::LEFT_PAREN, "Expected ( after function name in function call.");

        std::vector<AST::ExprVariant> args;
        if (!check(TokenType::RIGHT_PAREN)) // if there are arguments
        {
            // get the arg first, then loop back for the next one if possible
            do 
            {
                args.push_back(parse_assignment()); // get each arg (expression)
            } while (check(TokenType::COMMA) && advance().type == TokenType::COMMA); // the second arg is just for cleanliness purposes
        }
        expect(TokenType::RIGHT_PAREN, "Expected ) after function arguments in function call.");
        return std::make_unique<AST::Call>(id.line, id, std::move(args));
    }

    return parse_primary(); // lowest level to go
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
    else if (check(TokenType::IDENTIFIER))
    {
        const auto val = advance();
        AST::Variable v{val.line, val};
        return v;
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
    ss << "Parsing failed! " << why << " on line: " << line << ".\n";
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


bool Parser::check(TokenType* types, std::size_t len, TokenType& found) const
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


const Token& Parser::advance()
{
    if (current_tok < tokens.size() - 1)
    {
        return tokens[current_tok++]; 
    }
    
    return tokens[tokens.size() - 1];
}
