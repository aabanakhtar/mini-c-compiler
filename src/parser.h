#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

struct TreePrinter : AST::ExprVisitor
{
    virtual void operator()(std::unique_ptr<AST::Binary>&);
    virtual void operator()(std::unique_ptr<AST::Equality>&);
    virtual void operator()(const AST::Literal&);
    virtual void operator()(std::unique_ptr<AST::Unary>&);
    virtual void operator()(std::unique_ptr<AST::Assignment>&);
    virtual void operator()(std::unique_ptr<AST::Call>&);
    virtual void operator()(const AST::Variable&);
    virtual void operator()(std::unique_ptr<AST::StructAccess>&);
    virtual void operator()(std::unique_ptr<AST::ArrayAccess>&);
};

#endif // PARSER_H