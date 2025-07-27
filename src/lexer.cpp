#include "lexer.h"
#include "error.h"
#include <iostream>
#include <sstream>

Lexer::Lexer(const std::string &file)
    : file(file)
{
}

const std::vector<Token>& Lexer::get_tokens()
{
    for (curr_char = 0; curr_char < file.size();) 
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
        case '%': tokens.push_back(Token{TokenType::PERCENT, "%", line}); break;

            /* SPECIAL CASES */
        case '!':
            if (check('='))
            {
                tokens.push_back(Token{TokenType::BANG_EQUAL, "!=", line});
            }
            else
            {
                tokens.push_back(Token{TokenType::BANG, "!", line});
            }
            break;

        case '>':
            if (check('='))
            {
                tokens.push_back(Token{TokenType::GREATER_EQUAL, ">=", line});
            }
            else
            {
                tokens.push_back(Token{TokenType::GREATER, ">", line});
            }
            break;

        case '<':
            if (check('='))
            {
                tokens.push_back(Token{TokenType::LESS_EQUAL, "<=", line});
            }
            else
            {
                tokens.push_back(Token{TokenType::LESS, "<", line});
            }
            break;

        case '+':
            if (check('='))
            {
                tokens.push_back(Token{TokenType::PLUS_EQUAL, "+=", line});
            }
            else
            {
                tokens.push_back(Token{TokenType::PLUS, "+", line});
            }
            break;

        case '-':
            if (check('='))
            {
                tokens.push_back(Token{TokenType::MINUS_EQUAL, "-=", line});
            }
            else
            {
                tokens.push_back(Token{TokenType::MINUS, "-", line});
            }
            break;

        case '*':
            if (check('='))
            {
                tokens.push_back(Token{TokenType::STAR_EQUAL, "*=", line});
            }
            else
            {
                tokens.push_back(Token{TokenType::STAR, "*", line});
            }
            break;

        case '/':
            if (check('='))
            {
                tokens.push_back(Token{TokenType::SLASH_EQUAL, "/=", line});
            }
            else if (check('/'))
            {
                // remove comment 
                while (curr_char < file.size() && consume() != '\n')
                {
                }
            }
            else
            {
                tokens.push_back(Token{TokenType::SLASH, "/", line});
            }
            break;
            

            /* MULTI CHAR TOKENS */
        default:

            /* IDENTIFIERS AND KW */
            if (isalpha(ch)) 
            {
                keyword(ch); break;
            }
            /* NUMBERS */
            else if (isdigit(ch))
            {
                number(); break;
            }
            /* STRINGS */
            else if (ch == '"')
            {
                string(); break;
            }

            std::ostringstream ss; 
            ss << "Unexpected token \"" << ch << "\" on line: " << line << "\n"; 
            report_err(std::cout, ss.str());
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

bool Lexer::check(char c)
{
    // look ahead one
    if (curr_char < file.size() && file[curr_char] == c)
    {
        ++curr_char;
        return true;
    }

    return false;
}

const char Lexer::consume()
{
    return file[curr_char++];
}

void Lexer::keyword(char seed)
{
    std::string result(1, seed);  
    while (curr_char < file.size() && isalnum(file[curr_char])) 
    {
        result += consume(); 
    }

    auto is_keyword = keyword_to_token_type.find(result);
    if (is_keyword != keyword_to_token_type.end())
    {
        tokens.push_back(Token{is_keyword->second, is_keyword->first, line});
        return; // put in a keyword instead
    }

    tokens.push_back(Token{TokenType::IDENTIFIER, result, line}); 
}

void Lexer::string()
{
    std::string result;  
    while (curr_char < file.size() && file[curr_char] != '"') 
    {
        result += consume(); 
    }

    if (curr_char >= file.size())
    {
        std::ostringstream ss; 
        ss << "Unterminated string literal on line: " << line << "\n";
        report_err(std::cout, ss.str()); 
        return;
    }

    tokens.push_back(Token{TokenType::STRING, result, line});

    consume(); // the outer quote
}

void Lexer::number()
{
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
