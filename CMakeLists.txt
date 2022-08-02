cmake_minimum_required(VERSION 3.16)
project(lox VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(USE_CLANG_TIDY "Use clang-tidy")

if (USE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif ()

set(LOX_SOURCES
        src/chunk.cc
        src/compiler.cc
        src/main.cc
        src/parser.cc
        src/scanner.cc
        src/vm.cc
        )

set(LOX_HEADERS
        src/chunk.h
        src/compiler.h
        src/parser.h
        src/scanner.h
        src/token.h
        src/value.h
        src/vm.h
        )

add_executable(lox ${LOX_SOURCES} ${LOX_HEADERS})
target_compile_features(lox PRIVATE cxx_std_17)
target_compile_options(lox PRIVATE -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion)
set_target_properties(lox PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)