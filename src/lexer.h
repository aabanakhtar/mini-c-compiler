#ifndef LEXER_H
#define LEXER_H 

#include <string>
#include <unordered_map>
#include <vector> 

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    COMMA,          // ,
    DOT,            // .
    SEMICOLON,      // ;
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    PERCENT,        // %
    LEFT_SBRACKET,
    RIGHT_SBRACKET, 
    // One or two character tokens
    BANG,           // !
    BANG_EQUAL,     // !=
    EQUAL,          // =
    EQUAL_EQUAL,    // ==
    GREATER,        // >
    GREATER_EQUAL,  // >=
    LESS,           // <
    LESS_EQUAL,     // <=
    PLUS_EQUAL,     // +=
    MINUS_EQUAL,    // -=
    STAR_EQUAL,     // *=
    SLASH_EQUAL,    // /=
    AND, 
    OR,

    // Literals
    IDENTIFIER,
    NUMBER,
    STRING,

    // Keywords
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    INT,
    CHAR,
    VOID,
    STRUCT,
    BREAK,
    CONTINUE,

    // End of file
    END_OF_FILE
};


struct Token 
{
    TokenType type; 
    std::string value;
    std::size_t line;  
};

void print_token(const Token& t); 
std::string stringify_token_type(const TokenType t); 

class Lexer 
{
public: 
    explicit Lexer(const std::string& file);

    const std::vector<Token>& get_tokens();
private:
    bool check(char c);
    const char consume(); 

    void keyword(char seed); 
    void string();
    void _char();  
    void number(char c);
private:
    std::string file; 
    std::vector<Token> tokens;
    std::size_t curr_char; 
    std::size_t line = 1; 

    inline static const std::unordered_map<std::string, TokenType> keyword_to_token_type = {
        {"if",       TokenType::IF},
        {"else",     TokenType::ELSE},
        {"while",    TokenType::WHILE},
        {"for",      TokenType::FOR},
        {"return",   TokenType::RETURN},
        {"int",      TokenType::INT},
        {"char",     TokenType::CHAR},
        {"void",     TokenType::VOID},
        {"struct",   TokenType::STRUCT},
        {"break",    TokenType::BREAK},
        {"continue", TokenType::CONTINUE}
    };
};

#endif