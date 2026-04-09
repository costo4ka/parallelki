cmake_minimum_required(VERSION 3.10)
project(MatrixVectorMult)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread")

add_executable(matrix_vec main.cpp)