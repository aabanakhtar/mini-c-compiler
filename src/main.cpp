#include <iostream> 
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <fstream> 
#include <sstream>
#include "compiler.h"
#include "semanalyzer.h"

int main(int argc, char* argv[]) 
{
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
    auto toks = lexer.lex();
    for (auto& tok : toks) 
    {
        print_token(tok); 
    } 

    if (get_err() == ErrorMode::ERR)
    {
        return 1;
    }

    Parser parser(lexer.get_tokens());
    auto expr = parser.get_program();

    if (get_err() == ErrorMode::ERR)
    {
        return 1;
    }

    SemanticAnalyzer analyzer;
    auto [ok, e] = analyzer.perform_analysis(expr[1]);
    if (!ok)
    {
        return 1;
    }

    //Codegen gen;
    //gen.test_expr_gen(e);
}
