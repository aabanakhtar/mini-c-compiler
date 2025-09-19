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
    std::cout << "Read file: \n" << file_contents << "\n"; 

    Lexer lexer(file_contents); 
    auto toks = lexer.lex();

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
    for (auto& s : expr)
    {
        auto [ok, _] = analyzer.perform_analysis(s);
        s = std::move(_);
        if (!ok) 
        {
            std::cout << "Compilation failed: Failed parsing!";
            return 1;
        }
    }

    Codegen gen;
    std::cout << "\n\nGenerating LLVM IR....\n\n";
    gen.compile_translation_unit(expr);

    return 0;
}
