include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 20)
project(VGED-ENGINE VERSION 0.0.1)

add_subdirectory(vendor)
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(application/example_game)