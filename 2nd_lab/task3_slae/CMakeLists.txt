cmake_minimum_required(VERSION 3.10)
project(task3_slae LANGUAGES CXX)

find_package(OpenMP REQUIRED)

add_executable(slae_v1 variant1.cpp)
target_compile_options(slae_v1 PRIVATE -O2)
target_link_libraries(slae_v1 PRIVATE OpenMP::OpenMP_CXX m)

add_executable(slae_v2 variant2.cpp)
target_compile_options(slae_v2 PRIVATE -O2)
target_link_libraries(slae_v2 PRIVATE OpenMP::OpenMP_CXX m)
