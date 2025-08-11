#ifndef SEMANALYZER_H
#define SEMANALYZER_H
#include "ast.h"

class SemanticAnalyzer
{
public:
    explicit SemanticAnalyzer(const AST::ExprVariant& variant);
};

#endif // SEMANALYZER_H