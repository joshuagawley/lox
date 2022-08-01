cmake_minimum_required(VERSION 3.16)
project(lox VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(LOX_SOURCES
        src/chunk.cc
        src/compiler.cc
        src/main.cc
        src/parser.cc
        src/scanner.cc
        src/token.cc
        src/vm.cc
        )

set(LOX_HEADERS
        src/chunk.h
        src/compiler.h
        src/parser.h
        src/scanner.h
        src/value.h
        src/token.h
        src/vm.h
        )

add_executable(lox ${LOX_SOURCES} ${LOX_HEADERS})
target_compile_features(lox PRIVATE cxx_std_17)
set_target_properties(lox PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)