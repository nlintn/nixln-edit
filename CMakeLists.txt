cmake_minimum_required(VERSION 3.19)
project(nixln-edit VERSION "0.1.0")
set(CMAKE_C_STANDARD 23)
add_compile_options(-g -Wall -Wextra -Wpedantic)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# nix workaround
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_subdirectory("src")

