cmake_minimum_required(VERSION 3.22.1) 
project(baby-c)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRCs 
    src/main.cpp
    src/lexer.cpp
    src/error.cpp
)

add_executable(${PROJECT_NAME} ${SRCs})

