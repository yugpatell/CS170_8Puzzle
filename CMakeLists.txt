cmake_minimum_required(VERSION 3.22)
project(170)

set(CMAKE_CXX_STANDARD 14)

include_directories(Puzzle)

add_executable(170
        Puzzle/Node.h
        Puzzle/Puzzle.h
        main.cpp)
