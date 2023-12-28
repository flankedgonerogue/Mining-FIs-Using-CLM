cmake_minimum_required(VERSION 3.27)
project(Project)

set(CMAKE_CXX_STANDARD 20)

add_executable(Project main.cpp
        src/Graph.cpp
        include/Graph.h)
