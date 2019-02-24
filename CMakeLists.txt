cmake_minimum_required(VERSION 3.10)
project(seed)

set(CMAKE_CXX_STANDARD 11)

add_executable(seed generator.c layers.c main.cpp)
add_executable(gen generator.c)
add_executable(layer layers.c )