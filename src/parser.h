#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

struct TreePrinter : AST::ExprVisitor
{
    size_t indent_level = 0; 
    void indent();

    virtual void operator()(std::unique_ptr<AST::Binary>&) override;
    virtual void operator()(const AST::Literal&) override;
    virtual void operator()(std::unique_ptr<AST::Unary>&) override;
    virtual void operator()(std::unique_ptr<AST::Assignment>&) override;
    virtual void operator()(std::unique_ptr<AST::Call>&) override;
    virtual void operator()(const AST::Variable&) override;
    virtual void operator()(std::unique_ptr<AST::StructAccess>&) override;
    virtual void operator()(std::unique_ptr<AST::ArrayAccess>&) override;
};

#endif // PARSER_H