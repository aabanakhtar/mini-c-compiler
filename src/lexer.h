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
    const char consume(); 
private:
    std::string file; 
    std::vector<Token> tokens;
    std::size_t curr_char; 
    std::size_t line = 1; 

    inline static const std::unordered_map<std::string, TokenType> keyword_to_token_type = {
        
    };
};

#endif