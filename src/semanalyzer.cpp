#include "semanalyzer.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include "error.h"
#include "parser.h"

bool SemanticAnalyzer::is_binary_op_valid(const TokenType operation, const std::string& left_t, const std::string& right_t) const
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
        if (const auto [type1, type2] = it->second; left_t == type1 && right_t == type2
             || left_t == type2 && right_t == type1)
        {
            return true;
        }
    }

    return false;
}

bool SemanticAnalyzer::is_unary_op_valid(const TokenType operation, const std::string& right_t) const
{
    const auto range = unary_operations_rules_LUT.equal_range(operation);
    if (range.first == range.second)
    {
        return false; // not key in range
    }

    for (auto it = range.first; it != range.second; ++it)
    {
        // if the types match
        if (const auto type = it->second; type == right_t)
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

    if (AST::get_type(s) != "const char*")
    {
        report_err(std::cout, "Expected string in print statement!");
        return {false, AST::StatementVariant{}};
    }

    // add rich information
    statement->value = std::move(s);
    return {ok, std::move(statement)};
}

std::pair<bool, AST::StatementVariant> SemanticAnalyzer::sanalyze(std::unique_ptr<AST::VariableDecl>& statement)
{
    auto [ok, s] = perform_analysis(statement->value);
    if (!ok)
    {
        return {false, AST::StatementVariant{}};
    }

    // TODO: check if there is a valid type with structs and stuff
    if (!types.contains(statement->type) && statement->type != "void")
    {
        report_err(std::cout, "Compiler todo: support more types. for int are supported");
        return {false, AST::StatementVariant{}};
    }

    if (AST::get_type(s) != statement->type)
    {
        report_err(std::cout, "Expected matching types in variable declaration");
        return {false, AST::StatementVariant{}};
    }

    // no duplicate variables TODO: refactor for shadowing in scopes and stuff
    const auto duplicate_exists = std::find_if(declared_variables.begin(), declared_variables.end(), [&](auto& x) -> bool
    {
        return x.name == statement->name;
    });

    if (duplicate_exists != declared_variables.end())
    {
        report_err(std::cout, "Expected a non-duplicate identifier for a variable.");
        return {false, AST::StatementVariant{}};
    }

    declared_variables.emplace_back(statement->name, current_scope_depth);
    declared_variable_types[statement->name] = statement->type;
    // add the $$$
    statement->value = std::move(s);
    return {true, std::move(statement)};
}

std::pair<bool, AST::StatementVariant> SemanticAnalyzer::sanalyze(std::unique_ptr<AST::ExpressionStatement>& statement)
{
    auto [ok, e] = perform_analysis(statement->expr); // pretty simple for this, mostly handled by other routines
    if (!ok)
    {
        return {false, AST::StatementVariant{}};
    }

    statement->expr = std::move(e); // add richer info $$$$$$
    return {true, std::move(statement)};
}

std::pair<bool, AST::StatementVariant> SemanticAnalyzer::sanalyze(std::unique_ptr<AST::IfElseStatement>& statement)
{
    auto [ok, rich_condition] = perform_analysis(statement->condition);
    if (!ok || AST::get_type(rich_condition) != "int")
    {
        report_err(std::cout, "Expected a integer as the condition for if condition!");
        return {false, AST::StatementVariant{}};
    }

    auto [if_body_ok, rich_if_body] = perform_analysis(statement->if_body);
    if (!if_body_ok)
    {
        return {false, AST::StatementVariant{}};
    }

    auto [else_body_ok, rich_else_body] = perform_analysis(statement->else_body);
    if (!else_body_ok)
    {
        return {false, AST::StatementVariant{}};
    }

    statement->condition = std::move(rich_condition); 
    statement->if_body = std::move(rich_if_body);
    statement->else_body = std::move(rich_else_body);

    return {true, std::move(statement)};
}

std::pair<bool, AST::StatementVariant> SemanticAnalyzer::sanalyze(std::unique_ptr<AST::WhileStatement>& s)
{
    auto [ok, rich_condition] = perform_analysis(s->condition);
    if (!ok || AST::get_type(rich_condition) != "int")
    {
        report_err(std::cout, "Expected a integer as the condition for while condition!");
        return {false, AST::StatementVariant{}};
    }

    auto [body_ok, rich_body] = perform_analysis(s->body);
    if (!body_ok)
    {
        return {false, AST::StatementVariant{}};
    }

    s->condition = std::move(rich_condition); 
    s->body = std::move(rich_body);

    return {true, std::move(s)};
}

std::pair<bool, AST::StatementVariant> SemanticAnalyzer::sanalyze(std::unique_ptr<AST::BlockStatement>& statement)
{
    ++current_scope_depth;

    for (auto &st : statement->statements)
    {
        auto [ok, s] = perform_analysis(st);
        if (!ok) return {false, AST::StatementVariant{}};

        st = std::move(s);
    }

    --current_scope_depth;
    // STL so goated; filter out the bad variables
    const auto new_end = std::remove_if(declared_variables.begin(), declared_variables.end(), [&](auto& x) -> bool
    {
        if (x.scope_depth > current_scope_depth) return true;

        return false;
    });
    // remove
    declared_variables.erase(new_end, declared_variables.end());
    
    return {true, std::move(statement)};
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(AST::Literal& lit) const
{
    // just add type info, not much else needed
    lit.result_type = AST::get_literal_type(lit);
    return std::make_pair(true, std::move(lit));
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(AST::Variable& var)
{
    const auto found_variable = std::find_if(declared_variables.begin(), declared_variables.end(), [&](auto& x) -> bool
    {
        return x.name == var.name.value;
    });

    // second check just to keep scopes, might be redundant tbh
    if (found_variable == declared_variables.end() && found_variable->scope_depth > current_scope_depth)
    {
        std::ostringstream ss;
        ss << "undefined variable: " << var.name.value << "\n";
        report_err(std::cout, ss.str());
        return {false, AST::Literal{0, 0}};
    }

    var.result_type = declared_variable_types[var.name.value];
    return {true, var};
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
    // look up the operation and see if it's not valid
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

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(std::unique_ptr<AST::Assignment>& asn)
{
    auto [lhs_ok, lhs_expr] = perform_analysis(asn->lhs);
    auto [rhs_ok, rhs_expr] = perform_analysis(asn->rhs);

    if (!lhs_ok || !rhs_ok)
    {
        return {false, AST::Literal{0, 0}};
    }

    if (get_type(lhs_expr) != get_type(rhs_expr))
    {
        std::ostringstream ss;
        ss << "Cannot match types in assignment expression on line: " << asn->line << "\n";
        report_err(std::cout, ss.str());
        return {false, AST::Literal{0, 0}};
    }

    if (!is_storable_location(lhs_expr))
    {
        report_err(std::cout, "Assignment must be to a writable location.\n");
        return {false, AST::Literal{0, 0}};
    }

    asn->lhs = std::move(lhs_expr);
    asn->rhs = std::move(rhs_expr);
    asn->result_type = get_type(asn->lhs);
    return {true, std::move(asn)};
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const std::unique_ptr<AST::Call>& call)
{
    const auto found_function = declared_functions.find(call->func_name.value); 
    // Check if the function exists
    if (found_function == declared_functions.end())
    {
        std::ostringstream ss;
        ss << "Function not declared: " << call->func_name.value << "\n";
        report_err(std::cout, ss.str());
        return {false, AST::Literal{0, 0}};
    }


}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const std::unique_ptr<AST::StructAccess>& sa)
{
}

std::pair<bool, AST::ExprVariant> SemanticAnalyzer::analyze(const std::unique_ptr<AST::ArrayAccess>& aa)
{
}

std::pair<bool, AST::DeclarationVariant> SemanticAnalyzer::danalyze(std::unique_ptr<AST::FunctionDeclaration> &declaration)
{
    current_function = declaration.get(); // for substatements to access

    auto duplicate_exists = declared_functions.find(declaration->name); // make sure no dup functions exist
    if (duplicate_exists != declared_functions.end())
    {
        report_err(std::cout, "Function with the same name already exists!");
        return {false, AST::DeclarationVariant{}};
    }

    declared_functions.insert(declaration->name);

    std::unordered_set<std::string> param_names; 
    for (auto& [ty, name] : declaration->params) 
    {
        if (param_names.contains(name)) 
        {
            report_err(std::cout, "Duplicate parameter names are not allowed!");
            return {false, AST::DeclarationVariant{}};
        }
        
        param_names.insert(name);
        // simulate variable scoping behavior for usage in the function body (gets removed by block filtering later)
        declared_variables.push_back({name, current_scope_depth + 1});
        declared_variable_types[name] = ty;

        if (!types.contains(ty)) 
        {
            report_err(std::cout, "Unsupported parameter type in function declaration!");
            return {false, AST::DeclarationVariant{}};
        }
    }

    if (!types.contains(declaration->return_type)) 
    {
        report_err(std::cout, "Unsupported return type in function declaration!");
        return {false, AST::DeclarationVariant{}};
    }

    auto as_statement = AST::StatementVariant{std::move(declaration->body)};
    auto [ok, rich_body] = perform_analysis(as_statement);
    if (!ok)
    {
        return {false, AST::DeclarationVariant{}};
    }

    current_function = nullptr; 
    // update with rich information
    declaration->body = std::move(std::get<std::unique_ptr<AST::BlockStatement>>(rich_body));
    
    // ensure there is at least one return statement (is this correct? no. I don't care)
    bool found_return = false;
    for (auto& statement : declaration->body->statements)
    {
        if (std::holds_alternative<std::unique_ptr<AST::ReturnStatement>>(statement)) 
        {
            found_return = true;
            break;
        }

    }

    if (declaration->return_type != "void" && !found_return)
    {
        report_err(std::cout, "Non-void function must have at least one return statement!");
        return {false, AST::DeclarationVariant{}};
    }
    
    return {true, std::move(declaration)};
}

std::pair<bool, AST::StatementVariant> SemanticAnalyzer::sanalyze(std::unique_ptr<AST::ReturnStatement> &statement)
{
    if (statement->value.has_value())
    {
        auto [ok, rich_expr] = perform_analysis(statement->value.value());
        if (!ok)
        {
            return {false, AST::StatementVariant{}};
        }

        if (AST::get_type(rich_expr) != current_function->return_type)
        {
            report_err(std::cout, "Return type does not match function return type!");
            return {false, AST::StatementVariant{}};
        }

        statement->value = std::move(rich_expr);
    }
    else if (current_function->return_type != "void")
    {
        report_err(std::cout, "Non-void function must return a value!");
        return {false, AST::StatementVariant{}};
    }

    return {true, std::move(statement)};
}
