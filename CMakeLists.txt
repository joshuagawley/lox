cmake_minimum_required(VERSION 3.22)
project(lox VERSION 0.0.0 LANGUAGES CXX)

add_executable(lox src/main.cc)
target_compile_features(lox PRIVATE cxx_std_17)
set_target_properties(lox PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)