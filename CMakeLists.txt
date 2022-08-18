# temp cmake file, will be changed!
cmake_minimum_required(VERSION 3.10)

project(VGED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(VGED ${SRC_FILES})

target_include_directories(VGED
    PUBLIC src
)

target_link_libraries(VGED

)

