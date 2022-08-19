cmake_minimum_required(VERSION 3.10)
project(ecs)

add_library(ecs src/ecs.hpp src/ecs.cpp)
target_include_directories(ecs PUBLIC src/)


