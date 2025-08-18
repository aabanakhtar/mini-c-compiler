#ifndef SEMANALYZER_H
#define SEMANALYZER_H

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
    bool is_binary_op_valid(TokenType operation, AST::LiteralType left, AST::LiteralType right) const;
    bool is_unary_op_valid(TokenType operation, AST::LiteralType right) const;

    // statement analyze
    std::pair<bool, AST::StatementVariant> sanalyze(std::unique_ptr<AST::PrintStatement>& statement);

    std::pair<bool, AST::ExprVariant> analyze(AST::Literal& lit) const;
    std::pair<bool, AST::ExprVariant> analyze(const AST::Variable& var) const;

    std::pair<bool, AST::ExprVariant> analyze(std::unique_ptr<AST::Binary>& bin);
    std::pair<bool, AST::ExprVariant> analyze(std::unique_ptr<AST::Unary>& un);
    std::pair<bool, AST::ExprVariant> analyze(const std::unique_ptr<AST::Assignment>& asn);
    std::pair<bool, AST::ExprVariant> analyze(const std::unique_ptr<AST::Call>& call);
    std::pair<bool, AST::ExprVariant> analyze(const std::unique_ptr<AST::StructAccess>& sa);
    std::pair<bool, AST::ExprVariant> analyze(const std::unique_ptr<AST::ArrayAccess>& aa);

private:
    // store grammar rules
    const std::unordered_multimap<TokenType, std::pair<AST::LiteralType, AST::LiteralType>>
        binary_operations_rules_LUT = {
        {TokenType::PLUS, {AST::LiteralType::INT, AST::LiteralType::INT}}
    };

    const std::unordered_multimap<TokenType,AST::LiteralType>
        unary_operations_rules_LUT = {
        {TokenType::MINUS, {AST::LiteralType::INT}},
        {TokenType::PLUS, {AST::LiteralType::INT}}
    };
};

#endif // SEMANALYZER_H
