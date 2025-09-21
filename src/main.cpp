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
    if (argc < 2)
    {
        std::cerr << "Usage: mini-c <source-file>\n";
        return 1;
    }

    const char* filename = argv[1];
    std::fstream file(filename);
    if (!file.is_open())
    {
        report_err(std::cout, "mini-c couldn't open translation unit for compilation!");
        return 1;
    }
    
    std::string file_contents = (std::ostringstream() << file.rdbuf()).str();

    Lexer lexer(file_contents); 
    auto toks = lexer.lex();

    if (get_err() == ErrorMode::ERR)
    {
        std::cerr << "Failed to lex the input file!\n";
        return 1;
    }

    Parser parser(lexer.get_tokens());
    auto expr = parser.get_program();

    if (get_err() == ErrorMode::ERR)
    {
        std::cerr << "Failed to parse the input file!\n";
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

    std::cout << "\n\n\033[1mGenerating LLVM IR....\033[0m\n\n";
    Codegen gen;
    gen.compile_translation_unit(expr);

    return 0;
}
