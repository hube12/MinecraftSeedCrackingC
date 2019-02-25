cmake_minimum_required(VERSION 3.10)
project(seed)

set(CMAKE_CXX_STANDARD 11)

add_executable(seed generator.hpp layers.hpp layers.cpp generator.cpp main.cpp)
add_executable(gen generator.cpp)
add_executable(layer layers.cpp)