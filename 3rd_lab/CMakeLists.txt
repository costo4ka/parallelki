cmake_minimum_required(VERSION 3.10)
project(client_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_NAME "g++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILER_NAME "clang++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "NVHPC")
    set(COMPILER_NAME "nvc++")
else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS -O2)
endif()

find_package(Threads REQUIRED)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${COMPILER_NAME})
file(MAKE_DIRECTORY ${OUTPUT_DIR})

add_executable(client_server main.cpp)
target_link_libraries(client_server PRIVATE Threads::Threads)
set_target_properties(client_server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
target_compile_features(client_server PRIVATE cxx_std_20)

add_executable(test_results test.cpp)
set_target_properties(test_results PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
target_compile_features(test_results PRIVATE cxx_std_20)
