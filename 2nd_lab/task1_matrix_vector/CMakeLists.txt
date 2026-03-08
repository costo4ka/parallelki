cmake_minimum_required(VERSION 3.10)
project(task1_matrix_vector LANGUAGES C)

find_package(OpenMP REQUIRED)

add_executable(matrix_vector matrix_vector.c)
target_compile_options(matrix_vector PRIVATE -O2)
target_link_libraries(matrix_vector PRIVATE OpenMP::OpenMP_C m)
