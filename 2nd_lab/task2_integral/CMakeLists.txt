cmake_minimum_required(VERSION 3.10)
project(task2_integral LANGUAGES C)

find_package(OpenMP REQUIRED)

add_executable(integrate integrate.c)
target_compile_options(integrate PRIVATE -O2)
target_link_libraries(integrate PRIVATE OpenMP::OpenMP_C m)
