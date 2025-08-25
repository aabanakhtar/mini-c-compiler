# mini-c
Mini-C is a compiler for a restricted subset of the C programming language, written in C++ using LLVM's backend.


# Todo

## ✅ Basics

- **Integer types**: `int`, `char`
- **Basic constants**: numeric literals, character literals
- **Arithmetic operators**: `+`, `-`, `*`, `/`
- **Comparison operators**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Assignment**: `=`
- **Compound statements**: `{ ... }`
- **Return statement**

## ✅ Expressions

- **Parentheses grouping**
- **Unary operators**: `-`, `+`, `!`
- **Precedence and associativity**
- **Function calls with arguments**
- **Type casts**: `((int)x)` *(optional early on)*

## ✅ Control Flow

- `if`, `else`
- `while`
- `for` *(optional)*
- `break`, `continue` *(optional)*
- `return` with and without value

## ✅ Functions

- **Function declaration and definition**
- **Function parameters**: passed by value
- **Function return types**: `int`, `void`
- **Function call expressions**

## ✅ Variables and Scoping

- **Local variable declaration**
- **Variable initialization**
- **Shadowing and block scope**
- **Global variables** *(optional)*

## ✅ Types and Type System

- **Basic types**: `int`, `char`, `void`
- **Implicit type conversion** (e.g., `char` to `int`)
- **Type checking** for expressions and assignments

## ✅ I/O and External Functions (via libc)

- **Calling external functions** like `printf`
- **Linking with standard library**

## ✅ Minimal Preprocessing *(Optional)*

- `#define` macros *(optional)*
- `#include` statements *(optional, for standard headers)*
