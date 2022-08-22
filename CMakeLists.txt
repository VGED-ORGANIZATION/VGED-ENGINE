include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 20)
project(VGED-ENGINE VERSION 0.0.1)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/Debug)
else (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/Release)
endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(vendor)
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(application/example_game)