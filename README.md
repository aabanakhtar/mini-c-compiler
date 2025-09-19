# mini-c
Mini-C is a compiler for a restricted subset of the C programming language, written in C++ using LLVM's backend. It is primarily built to learn how the LLVM backend cooperates with a frontend. 

Example Input: 
```
int add(int a, int b)
{
        return a + b;
}


int main() {
        return add(2, 3);
}
```

Corresponding Output:
```
; ModuleID = 'main'
source_filename = "main"

define i32 @add(i32 %a, i32 %b) {
entry:
  %a.addr = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  %b.addr = alloca i32, align 4
  store i32 %b, ptr %b.addr, align 4
  %loadedVal = load i32, ptr %a.addr, align 4
  %loadedVal1 = load i32, ptr %b.addr, align 4
  %0 = add i32 %loadedVal, %loadedVal1
  ret i32 %0
}

define i32 @main() {
entry:
  %callresult = call i32 @add(i32 2, i32 3)
  ret i32 %callresult
}
```

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
sudo apt-get install llvm-dev clang
```

Then, build the CMake Project
```sh
mkdir build
cd build
cmake -G .. 
cmake --build . 
```

Then, run the project:

```sh
./mini-c-compiler main.c 
``` 

The project will spit out an llvm IR file, which you can compile using clang. 
