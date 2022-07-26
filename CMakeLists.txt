cmake_minimum_required(VERSION 3.22)
project(lox VERSION 0.0.0 LANGUAGES CXX)

set(LOX_SOURCES
        src/main.cc
        src/chunk.cc
        )

set(LOX_HEADERS
        src/chunk.h
        src/value.h
        )

add_executable(lox ${LOX_SOURCES} ${LOX_HEADERS})
target_compile_features(lox PRIVATE cxx_std_17)
set_target_properties(lox PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)