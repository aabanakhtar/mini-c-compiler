#include <iostream> 
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <fstream> 
#include <sstream>
#include "compiler.h"

int main(int argc, char* argv[]) 
{
    #if 0
    if (argc != 2) 
    {
        report_err(std::cout, "Expected a filename!"); 
        return 1;
    }

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
    
    Parser parser(lexer.get_tokens());
    auto expr = parser.get_program();
    std::visit(TreePrinter{}, expr); 

    #endif 

    AST::Literal lit(0, 0.5); 
    Codegen gen;
    gen.test_literal_codegen();
}