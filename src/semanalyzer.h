#ifndef SEMANALYZER_H
#define SEMANALYZER_H

#include <unordered_set>

#include "ast.h"

class SemanticAnalyzer
{
public:
    explicit SemanticAnalyzer() = default;

    auto perform_analysis(AST::ExprVariant& variant)
    {
        return std::visit([&](auto& v)
        {
            return analyze(v);
        }, variant);
    }

    auto perform_analysis(AST::StatementVariant& variant)
    {
        return std::visit([&](auto& v)
        {
            return sanalyze(v);
        }, variant);
    }

private:
    bool is_binary_op_valid(TokenType operation, const std::string& left_t, const std::string& right_t) const;
    bool is_unary_op_valid(TokenType operation, const std::string& right_t) const;
    // check whether the area is writable memory
    // TODO: support pointers??
    bool is_storable_location(AST::ExprVariant& variant) const
    {
        return std::visit([&]<typename T0>(T0&& x)
        {
            using T = std::decay_t<T0>;
            if (std::is_same_v<T, AST::Variable>)
            {
                return true;
            }

            return false;
        }, variant);
    }

    // statement analyze
    std::pair<bool, AST::StatementVariant> sanalyze(std::unique_ptr<AST::PrintStatement>& statement);
    std::pair<bool, AST::StatementVariant> sanalyze(std::unique_ptr<AST::VariableDecl>& statement);
    std::pair<bool, AST::StatementVariant> sanalyze(std::unique_ptr<AST::ExpressionStatement>& statement);
    std::pair<bool, AST::StatementVariant> sanalyze(std::unique_ptr<AST::IfElseStatement>& statement);

    std::pair<bool, AST::ExprVariant> analyze(AST::Literal& lit) const;
    std::pair<bool, AST::ExprVariant> analyze(AST::Variable& var);
    std::pair<bool, AST::ExprVariant> analyze(std::unique_ptr<AST::Binary>& bin);
    std::pair<bool, AST::ExprVariant> analyze(std::unique_ptr<AST::Unary>& un);
    std::pair<bool, AST::ExprVariant> analyze(std::unique_ptr<AST::Assignment>& asn);
    std::pair<bool, AST::ExprVariant> analyze(const std::unique_ptr<AST::Call>& call);
    std::pair<bool, AST::ExprVariant> analyze(const std::unique_ptr<AST::StructAccess>& sa);
    std::pair<bool, AST::ExprVariant> analyze(const std::unique_ptr<AST::ArrayAccess>& aa);

private:
    std::unordered_set<std::string> declared_variables;
    std::unordered_map<std::string, std::string> declared_variable_types;

    // store grammar rules
    const std::unordered_multimap<TokenType, std::pair<std::string, std::string>>
        binary_operations_rules_LUT = {
            {TokenType::PLUS, {"int", "int"}},
            {TokenType::MINUS, {"int", "int"}},
        };

    const std::unordered_multimap<TokenType, std::string>
        unary_operations_rules_LUT = {
            {TokenType::MINUS, "int"},
            {TokenType::PLUS, "int"}
    };
};

#endif // SEMANALYZER_H
