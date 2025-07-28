#include <iostream> 
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <fstream> 
#include <sstream>

int main(int argc, char* argv[]) 
{
    #if 0
    if (argc != 2) 
    {
        report_err(std::cout, "Expected a filename!"); 
        return 1;
    }
    #endif 

    const char* filename = "../test/main.c";
    std::fstream file(filename);
    if (!file.is_open())
    {
        report_err(std::cout, "Couldn't open translation unit for compilation!");
        return 1;
    }
    std::string file_contents = (std::ostringstream() << file.rdbuf()).str();
    std::cout << file_contents << "\n"; 

    Lexer lexer(file_contents); 
    auto toks = lexer.get_tokens(); 
    for (auto& tok : toks) 
    {
        print_token(tok); 
    } 

    AST::Literal r;
    r.line = 4; 
    r.value = 6;  

    auto q = std::make_unique<AST::Binary>(
    );
    q->line = 0; 
    q->left = r;
    q->right = r; 
    q->op = TokenType::PLUS; 
    AST::ExprVariant e0 = std::move(q); 
 
    auto b = std::make_unique<AST::Binary>(
    );
    b->line = 0; 
    b->left = std::move(e0);
    b->right =r; 
    b->op = TokenType::PLUS; 
    AST::ExprVariant e = std::move(b); 
    TreePrinter p; 
    std::visit(p, e);

    return 0; 
}