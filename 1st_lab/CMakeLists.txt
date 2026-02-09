cmake_minimum_required(VERSION 3.10)
project(Lab1)

option(USE_DOUBLE "Use double precision instead of float" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Lab1 main.cpp)

if(USE_DOUBLE)
    target_compile_definitions(Lab1 PRIVATE USE_DOUBLE)
endif()