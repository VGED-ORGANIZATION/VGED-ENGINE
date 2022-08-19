cmake_minimum_required(VERSION 3.11.0)
project(Editor VERSION 0.0.1)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(Editor ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(Editor LINK_PUBLIC Engine)