cmake_minimum_required(VERSION 3.22.1) 
project(baby-c)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

set(SRCs 
    src/main.cpp
    src/lexer.cpp
    src/error.cpp
    src/parser.cpp
    src/compiler.cpp
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(${PROJECT_NAME} ${SRCs})
target_link_libraries(${PROJECT_NAME} ${llvm_libs})

