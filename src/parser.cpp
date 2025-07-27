#include "parser.h"
#include <iostream>

void TreePrinter::operator()(std::unique_ptr<AST::Binary>& bin)
{
    std::cout << "Binary (op = " << static_cast<int>(bin->op) << ")\n";
    std::cout << "Left: ";
    std::visit(*this, bin->left);
    std::cout << "\nRight: ";
    std::visit(*this, bin->right);
    std::cout << "\n";
}

void TreePrinter::operator()(std::unique_ptr<AST::Unary>& unary)
{
    std::cout << "Unary (op = " << static_cast<int>(unary->op) << ")\n";
    std::cout << "Operand: ";
    std::visit(*this, unary->operand);
    std::cout << "\n";
}

void TreePrinter::operator()(std::unique_ptr<AST::Assignment>& assign)
{
    std::cout << "Assignment (op = " << static_cast<int>(assign->op) << ")\n";
    std::cout << "LHS: ";
    std::visit(*this, assign->lhs);
    std::cout << "\nRHS: ";
    std::visit(*this, assign->rhs);
    std::cout << "\n";
}

void TreePrinter::operator()(std::unique_ptr<AST::Call>& call)
{
    std::cout << "Call (func = " << call->func_name.value << ")\n";
    std::cout << "Args:\n";
    for (auto& arg : call->args)
    {
        std::visit(*this, arg);
        std::cout << "\n";
    }
}

void TreePrinter::operator()(const AST::Variable& var)
{
    std::cout << "Variable (" << var.name.value << ")";
}

void TreePrinter::operator()(std::unique_ptr<AST::StructAccess>& sa)
{
    std::cout << "StructAccess (member = " << sa->member_name << ")\n";
    std::cout << "Object: ";
    std::visit(*this, sa->lhs);
    std::cout << "\n";
}

void TreePrinter::operator()(std::unique_ptr<AST::ArrayAccess>& aa)
{
    std::cout << "ArrayAccess\n";
    std::cout << "Array: ";
    std::visit(*this, aa->lhs);
    std::cout << "\nIndex: ";
    std::visit(*this, aa->index);
    std::cout << "\n";
}

void TreePrinter::operator()(const AST::Literal& lit)
{
    std::visit([](auto&& val) { std::cout << "Literal (" << val << ")"; }, lit.value);
}