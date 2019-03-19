cmake_minimum_required(VERSION 3.10)
project(seed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_XX_FLAGS}")

add_executable(seed generator.hpp layers.hpp layers.cpp generator.cpp main.cpp)
