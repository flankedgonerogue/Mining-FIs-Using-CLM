cmake_minimum_required(VERSION 3.27)
project(Mining-FIs-Using-CLM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "-Oz -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

add_executable(clm-miner main.cpp
        src/Graph.cpp
        include/Graph.hpp)
