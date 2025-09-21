#include "compiler.h"

#include <iostream>

Codegen::Codegen()
{
    context = std::make_unique<llvm::LLVMContext>();
    mod = std::make_unique<llvm::Module>("main", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);

    // map default types
    type_to_llvm_ty["int"]   = llvm::Type::getInt32Ty(*context);
    type_to_llvm_ty["char"]  = llvm::Type::getInt8Ty(*context);
    type_to_llvm_ty["void"]  = llvm::Type::getVoidTy(*context);
}

void Codegen::compile_translation_unit(const std::vector<AST::DeclarationVariant> &declarations)
{
    for (auto & d : declarations)
    {
        generate(d);
    }

    mod->print(llvm::outs(), nullptr);
    // check if its generating good IR
    if (llvm::verifyModule(*mod, &llvm::errs()))
    {
        llvm::errs() << "Module verification failed! Please consider this a severe skill issue.\n";
    }
}


llvm::Instruction* Codegen::sgen(const std::unique_ptr<AST::BlockStatement>& block)
{
    for (const auto& s : block->statements)
    {
        generate(s);
    }

    return nullptr; 
}

llvm::Instruction* Codegen::sgen(const std::unique_ptr<AST::PrintStatement>& s)
{
    auto str_arg = std::get<AST::Literal>(s->value);
    const auto as_str = std::get<std::string>(str_arg.value);
    llvm::Value* as_llvm = builder->CreateGlobalStringPtr(as_str);
    return builder->CreateCall(printf_decl(), {as_llvm});
}

llvm::Instruction* Codegen::sgen(const std::unique_ptr<AST::VariableDecl>& a)
{
    const auto alloca = builder->CreateAlloca(type_to_llvm_ty[a->type], nullptr, a->name);
    llvm::Value* evaluated = generate(a->value);
    variable_locations[a->name] = alloca;
    builder->CreateStore(evaluated, alloca);
    return alloca;
}

llvm::Instruction* Codegen::sgen(const std::unique_ptr<AST::ExpressionStatement>& e)
{
    generate(e->expr);
    return nullptr; // will this bite me
}

llvm::Function* Codegen::dgen(const std::unique_ptr<AST::FunctionDeclaration> &fd)
{
    auto return_type = type_to_llvm_ty[fd->return_type];

    auto arg_types = std::vector<llvm::Type*>{};
    for (const auto& arg : fd->params)
    {
        arg_types.push_back(type_to_llvm_ty[arg.type]);
    }   

    llvm::Function* func = llvm::Function::Create(
        llvm::FunctionType::get(return_type, arg_types, false), // false = not vararg
        llvm::Function::ExternalLinkage,
        fd->name,
        *mod
    );
    
    // add the names to the arguments
    for (auto& arg : func->args())
    {
        arg.setName(fd->params[arg.getArgNo()].name);
    }

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(bb);    
 
    // promote arguments to variables and store them
    for (auto& arg : func->args())
    {
        const auto alloca = builder->CreateAlloca(arg.getType(), nullptr, arg.getName() + "_asalloca");
        builder->CreateStore(&arg, alloca);
        variable_locations[arg.getName().str()] = alloca;
    }
    
    generate(AST::StatementVariant{std::move(fd->body)});

    if (fd->return_type == "void")
    {
        builder->CreateRetVoid();
    }

    declared_functions[fd->name] = func;
    return func;
}

llvm::Instruction *Codegen::sgen(const std::unique_ptr<AST::ReturnStatement> &r)
{
    if (r->value.has_value()) {
        llvm::Value* ret_val = generate(r->value.value());
        return builder->CreateRet(ret_val);
    } else {
        return builder->CreateRetVoid();
    }
}

llvm::Instruction* Codegen::sgen(const std::unique_ptr<AST::IfElseStatement>& e)
{
    // any value that is non-zero is true in C
    static auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0);
    auto condition_value = generate(e->condition); 

    auto condition = builder->CreateICmpNE(condition_value, zero, "ifcond");

    llvm::Function* current_function = builder->GetInsertBlock()->getParent();
    // define the bodies for each branch
    auto if_block = llvm::BasicBlock::Create(*context, "ifbody", current_function);
    auto else_block = llvm::BasicBlock::Create(*context, "elsebody", current_function); 
    auto merge_block = llvm::BasicBlock::Create(*context, "ifend", current_function);

    // branch to the correct block based on the condition
    builder->CreateCondBr(condition, if_block, else_block); 

    // move to the if body
    builder->SetInsertPoint(if_block);
    generate(e->if_body);
    // jump back to the merge point to continue normal execution
    builder->CreateBr(merge_block);
    if_block = builder->GetInsertBlock(); // update the current block (llvm internal thing?)

    // create else body
    builder->SetInsertPoint(else_block);
    generate(e->else_body); 
    // return control flow (to merge point)
    builder->CreateBr(merge_block);
    else_block = builder->GetInsertBlock(); // update current block (llvm internal thing?)

    // create the merge point
    builder->SetInsertPoint(merge_block);

    return nullptr;
}

llvm::Instruction* Codegen::sgen(const std::unique_ptr<AST::WhileStatement>& w)
{
    // any value that is non-zero is true in C
    static auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0);
    
    // define our basic blocks
    llvm::Function* current_function = builder->GetInsertBlock()->getParent();
    auto while_cond = llvm::BasicBlock::Create(*context, "whilecond", current_function);
    auto body_block = llvm::BasicBlock::Create(*context, "whilebody", current_function);
    auto merge_point = llvm::BasicBlock::Create(*context, "whileend", current_function);
    
    builder->CreateBr(while_cond); // jump to condition check first
    
    builder->SetInsertPoint(while_cond);
    auto condition = generate(w->condition); 
    auto condition_as_bool = builder->CreateICmpNE(condition, zero, "whilecond");
    builder->CreateCondBr(condition_as_bool, body_block, merge_point);    
    while_cond = builder->GetInsertBlock(); // update current block (llvm internal thing?)

    // generate the body
    builder->SetInsertPoint(body_block);
    generate(w->body);
    builder->CreateBr(while_cond);
    // update and exit loop 
    body_block = builder->GetInsertBlock(); // update current block (llvm internal thing?)
    
    builder->SetInsertPoint(merge_point); // continue normal execution
    return nullptr; 
}

llvm::Value *Codegen::gen(const AST::Literal &lit)
{
    auto generate_ir = [&]<typename T0>(T0&& literal) -> llvm::Value*
    {
        using T = std::decay_t<T0>; // get the underlying type

        if (std::is_same_v<T, float>)
        {
            // get the type if it exists or create it in the context
            auto type = llvm::Type::getFloatTy(*context); 
            // generate it
            return llvm::ConstantFP::get(type, literal); 
        } 
        else if (std::is_same_v<T, double>)
        {
            auto type = llvm::Type::getDoubleTy(*context); 
            return llvm::ConstantFP::get(type, literal); // doubles work with constant FP
        }
        else if (std::is_same_v<T, int>)
        {
            auto type = llvm::Type::getInt32Ty(*context); 
            return llvm::ConstantInt::get(type, literal, true); // true = signed
        } 
        else if (std::is_same_v<T, char>)
        {
            auto type = llvm::Type::getInt8Ty(*context); 
            return llvm::ConstantInt::get(type, literal, true);
        }

        return nullptr; 
    };


    return std::visit(generate_ir, lit.value);
}


llvm::Value* Codegen::gen(const AST::Variable& var)
{
    const auto allocation = variable_locations[var.name.value];
    llvm::Value* loadedVal = builder->CreateLoad(
        allocation->getAllocatedType(),  // type of value stored
        allocation,                       // pointer to load from
        "load" + var.name.value                        // optional name
    );

    // if we are just getting the address of the variable, return that
    if (!value_flag)
    {
        return allocation; 
    }

    return loadedVal;
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Binary>& bin)
{
    if (bin->result_type != "int") return nullptr; // TODO: fix this to support more types

    return generate_int_ops(bin);
}

llvm::Value* Codegen::generate_unary_int_ops(const std::unique_ptr<AST::Unary>& un)
{
    const auto rhs = generate(un->operand);

    switch (un->op)
    {
    case TokenType::MINUS:
    {
        const auto type = llvm::Type::getInt32Ty(*context);
        const auto zero_val = llvm::ConstantInt::get(type, 0, true);
        return builder->CreateSub(zero_val, rhs, "negatetmp");
    }
    case TokenType::PLUS:
        return rhs; // unary + has no effect on the operand
    default:
        return nullptr;
    }
}

llvm::Value* Codegen::generate_unary_double_ops(const std::unique_ptr<AST::Unary>& un)
{
    const auto rhs = generate(un->operand);

    switch (un->op)
    {
    case TokenType::MINUS:
    {
        const auto type = llvm::Type::getDoubleTy(*context);
        const auto zero_val = llvm::ConstantFP::get(type, 0.0);
        return builder->CreateFSub(zero_val, rhs);
    }
    case TokenType::PLUS:
        return rhs; // unary + has no effect on the operand
    default:
        return nullptr;
    }
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Unary>& un)
{
    if (un->result_type != "int") return nullptr; // TODO: add more types

    return generate_unary_int_ops(un);
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Assignment>& asn)
{
    const auto rhs = generate(asn->rhs);
    value_flag = false;
    const auto lhs = generate(asn->lhs);
    value_flag = true;
    builder->CreateStore(rhs, lhs);
    return rhs; 
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::Call>& call)
{
    std::vector<llvm::Value*> args; 
    for (auto& arg : call->args)
    {
        args.push_back(generate(arg));
    }

    return builder->CreateCall(declared_functions[call->func_name.value], args, "callresult");
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::StructAccess>& sa)
{
    return nullptr;
}

llvm::Value* Codegen::gen(const std::unique_ptr<AST::ArrayAccess>& aa)
{
    return nullptr;
}

llvm::Value* Codegen::generate_int_ops(const std::unique_ptr<AST::Binary>& bin)
{
    const auto left = generate(bin->left);
    const auto right = generate(bin->right);

    if (!left || !right)
    {
        return nullptr;
    }

    llvm::Value* result = nullptr; // store i1 before casting

    switch (bin->op)
    {
    case TokenType::PLUS:
        result = builder->CreateAdd(left, right, "plustmp");
        break;
    case TokenType::MINUS:
        result = builder->CreateSub(left, right, "minustmp");
        break;
    case TokenType::STAR:
        result = builder->CreateMul(left, right, "multmp");
        break;
    case TokenType::SLASH:
        result = builder->CreateSDiv(left, right, "divtmp");
        break;
    case TokenType::EQUAL_EQUAL:
        result = builder->CreateICmpEQ(left, right, "eqtmp");
        break;
    case TokenType::BANG_EQUAL:
        result = builder->CreateICmpNE(left, right, "bang_eqtmp");
        break;
    case TokenType::LESS:
        result = builder->CreateICmpSLT(left, right, "lesstmp");
        break;
    case TokenType::GREATER:
        result = builder->CreateICmpSGT(left, right, "greatertmp");
        break;
    case TokenType::LESS_EQUAL:
        result = builder->CreateICmpSLE(left, right, "lesseqtmp");
        break;
    case TokenType::GREATER_EQUAL:
        result = builder->CreateICmpSGE(left, right, "greatereqtmp");
        break;

    case TokenType::AND:
        result = builder->CreateAnd(left, right, "andtmp");
        break;
    case TokenType::OR:
        result = builder->CreateOr(left, right, "ortmp");
        break;
    default:
        return nullptr;
    }

    // Force cast to sign i32 before returning
    return builder->CreateSExtOrBitCast(result, llvm::Type::getInt32Ty(*context));
}

llvm::Value* Codegen::generate_precise_ops(const std::unique_ptr<AST::Binary>& bin)
{
    const auto left = generate(bin->left);
    const auto right = generate(bin->right);

    if (!left || !right)
    {
        return nullptr;
    }

    switch (bin->op)
    {
    case TokenType::PLUS:
        return builder->CreateFAdd(left, right);
    case TokenType::MINUS:
        return builder->CreateFSub(left, right);
    case TokenType::STAR:
        return builder->CreateFMul(left, right);
    case TokenType::SLASH:
        return builder->CreateFDiv(left, right);
    // c is ordered, meaning that NaN is treated as a bogus value and is never equal to anything
    case TokenType::EQUAL_EQUAL:
        return builder->CreateFCmpOEQ(left, right); // integer cmp eq
    case TokenType::BANG_EQUAL:
        return builder->CreateFCmpONE(left, right); // integer cmp !eq what are these names vro
    case TokenType::AND:
    case TokenType::OR: 
    default:
        return nullptr; // shouldn't reach here unless something is weird
    }
}
