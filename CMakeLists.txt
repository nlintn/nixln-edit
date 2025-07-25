cmake_minimum_required(VERSION 3.19)
project(nixln-edit VERSION "0.1.0")
set(CMAKE_C_STANDARD 23)

set(CMAKE_DEBUG_POSTFIX "-debug")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -fsanitize=address -fsanitize=bounds -fsanitize=undefined")

add_compile_options(-Wall -Wextra -Wpedantic)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("src")

