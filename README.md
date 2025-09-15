# mini-c
Mini-C is a compiler for a restricted subset of the C programming language, written in C++ using LLVM's backend. It is primarily built to learn how the LLVM backend cooperates with a frontend. 

## Features
- Simple lexer based on a large switch
- Recursive Descent Parsing using `std::variant`
- Semantic Analysis using recursive descent
- Supports core C language features:
  - `int` type
  - `while`, `if`, and `else` statements
  - Function declarations of type `int` and `void`

## Requirements
- C++20 compiler (tested with GCC and Clang)
- LLVM (version 15 or newer recommended)
- CMake

## Building
First, ensure LLVM is installed and available via `find_package`.  
On Linux, you may need to install LLVM development packages, for example:

```sh
sudo apt install llvm-dev clang
```

more coming soon.
