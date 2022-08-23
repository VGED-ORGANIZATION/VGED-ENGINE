cmake_minimum_required(VERSION 3.11.0)
project(editor VERSION 0.0.1)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_NAME}
    ../engine
    ../vendor/spdlog/include
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC engine)