#include "lexer.h"
#include "error.h"
#include <iostream>


Lexer::Lexer(const std::string &file)
    : file(file)
{
}

const std::vector<Token>& Lexer::get_tokens()
{
    for (curr_char = 0; curr_char < file.size() - 1;) 
    {
        switch (auto ch = consume())
        {
            case '\n':
                ++line; 
                break;
                
            case ' ':
            case '\r':
            case '\t':
                break; // skip 
            
            /* SINGLE CHARACTER TOKENS */                

            case '(': tokens.push_back(Token{TokenType::LEFT_PAREN, "(", line}); break;
            case ')': tokens.push_back(Token{TokenType::RIGHT_PAREN, ")", line}); break;
            case '{': tokens.push_back(Token{TokenType::LEFT_BRACE, "{", line}); break;
            case '}': tokens.push_back(Token{TokenType::RIGHT_BRACE, "}", line}); break;
            case ',': tokens.push_back(Token{TokenType::COMMA, ",", line}); break;
            case '.': tokens.push_back(Token{TokenType::DOT, ".", line}); break;
            case ';': tokens.push_back(Token{TokenType::SEMICOLON, ";", line}); break;
            case '+': tokens.push_back(Token{TokenType::PLUS, "+", line}); break;
            case '-': tokens.push_back(Token{TokenType::MINUS, "-", line}); break;
            case '*': tokens.push_back(Token{TokenType::STAR, "*", line}); break;
            case '/': tokens.push_back(Token{TokenType::SLASH, "/", line}); break;
            case '%': tokens.push_back(Token{TokenType::PERCENT, "%", line}); break;
            case '!': tokens.push_back(Token{TokenType::BANG, "!", line}); break; 
            /* MULTI CHAR TOKENS*/

            default:
                report_err(std::cout, "Unexpected token!");
                break;
        }
    }


    tokens.push_back(Token{
        .type = TokenType::END_OF_FILE, 
        .value = "",
        .line = line, 
    });

    return tokens;
}

const char Lexer::consume()
{
    return file[curr_char++];
}

void print_token(const Token &t)
{
    std::cout << "Token(" 
            << "Type: " << stringify_token_type(t.type) << ", "
            << "Value: \"" << t.value << "\", "
            << "Line: " << t.line 
            << ")\n";
}

std::string stringify_token_type(const TokenType type)
{
    switch (type)
    {
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::PLUS_EQUAL: return "PLUS_EQUAL";
        case TokenType::MINUS_EQUAL: return "MINUS_EQUAL";
        case TokenType::STAR_EQUAL: return "STAR_EQUAL";
        case TokenType::SLASH_EQUAL: return "SLASH_EQUAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::RETURN: return "RETURN";
        case TokenType::INT: return "INT";
        case TokenType::CHAR: return "CHAR";
        case TokenType::VOID: return "VOID";
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::BREAK: return "BREAK";
        case TokenType::CONTINUE: return "CONTINUE";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}
