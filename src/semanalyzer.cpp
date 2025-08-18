#include "semanalyzer.h"

#include <iostream>
#include <sstream>

#include "error.h"

bool SemanticAnalyzer::is_binary_op_valid(TokenType operation, AST::LiteralType left, AST::LiteralType right) const
{
    const auto range = binary_operations_rules_LUT.equal_range(operation);
    // key doesn't exist ig
    if (range.first == range.second)
    {
        return false;
    }

    for (auto it = range.first; it != range.second; ++it)
    {
        // check if an existing entry exists
        if (const auto [type1, type2] = it->second; left == type1 && right == type2
             || left == type2 && right == type1)
        {
            return true;
        }
    }

    return false;
}

bool SemanticAnalyzer::is_unary_op_valid(TokenType operation, AST::LiteralType right) const
{
    const auto range = unary_operations_rules_LUT.equal_range(operation);
    if (range.first == range.second)
    {
        return false; // not key in range
    }

    for (auto it = range.first; it != range.second; ++it)
    {
        // if the types match
        if (const auto type = it->second; type == right)
        {
            return true;
        }
    }

    return false;
}

std::pair<bool, AST::StatementVariant> SemanticAnalyzer::sanalyze(std::unique_ptr<AST::PrintStatement>& statement)
{
    auto [ok, s] = perform_analysis(statement->value);
    if (!ok)
    {
        // error already addressed
        return {false, AST::StatementVariant{}};
    }

    if (AST::get_type(s) != AST::LiteralType::CSTRING)
    {
        report_err(std::cout, "Expected string in print statement!");
        return {false, AST::StatementVariant{}};
    }

    // add rich information
    statement->value = std::move(s);
    return {ok, std::move(statement)};
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(AST::Literal& lit) const
{
    // just add type info, not much else needed
    lit.result_type = AST::get_literal_type(lit);
    return std::make_pair(true, std::move(lit));
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const AST::Variable& var) const
{
    abort();
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(std::unique_ptr<AST::Binary>& bin)
{
    auto [l_ok, expr_1] = perform_analysis(bin->left);
    auto [r_ok, expr_2] = perform_analysis(bin->right);

    // if lower analysis failed
    if (!l_ok || !r_ok)
    {
        return {false, AST::Literal{0, 0}};
    }
    // look up the operation and see if its not valid
    if (!is_binary_op_valid(bin->op, AST::get_type(expr_1), AST::get_type(expr_2)))
    {
        std::ostringstream ss;
        ss << "Semantic analysis failed! Non matching types on binary operation in line: " << bin->line << "\n";
        report_err(std::cout, ss.str());
        return {false, AST::Literal{0, 0}};
    }

    // update the tree with rich info
    bin->left = std::move(expr_1);
    bin->right = std::move(expr_2);
    // TODO: upcasting
    bin->result_type = AST::get_type(bin->left); // for further analysis
    return {true, std::move(bin)};
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(std::unique_ptr<AST::Unary>& un)
{
    auto [ok, expr] = perform_analysis(un->operand);
    if (!ok)
    {
        return {false, AST::Literal{0, 0}}; // failed somewhere down lower in the tree
    }

    if (!is_unary_op_valid(un->op, AST::get_type(expr)))
    {
        std::ostringstream ss;
        ss << "Semantic analysis failed! Non matching types on unary operation in line: " << un->line << "\n";
        report_err(std::cout, ss.str());
        return {false, AST::Literal{0, 0}};
    }

    un->operand = std::move(expr);
    un->result_type = AST::get_type(un->operand); // for now
    return {true, std::move(un)};
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const std::unique_ptr<AST::Assignment>& asn)
{
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const std::unique_ptr<AST::Call>& call)
{
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const std::unique_ptr<AST::StructAccess>& sa)
{
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const std::unique_ptr<AST::ArrayAccess>& aa)
{
}
