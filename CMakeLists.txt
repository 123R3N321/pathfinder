cmake_minimum_required(VERSION 3.27)
project(pathfinder)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(pathfinder
        heuristic.h
        main.cpp)
