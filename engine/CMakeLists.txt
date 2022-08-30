cmake_minimum_required(VERSION 3.11.0)
project(engine VERSION 0.0.1)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

include_directories(${PROJECT_NAME}
    ./
    ../vendor/
    ../vendor/spdlog/include/
)

target_link_libraries(${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    glfw
    glm
    volk
    VulkanMemoryAllocator
    shaderc
    imgui
    tinygltf
    discord-rpc
)

# ############## Build SHADERS #######################

# # Find all vertex and fragment sources within shaders directory
# # taken from VBlancos vulkan tutorial
# # https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
# find_program(GLSL_VALIDATOR glslangValidator HINTS
#   ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
#   /usr/bin
#   /usr/local/bin
#   ${VULKAN_SDK_PATH}/Bin
#   ${VULKAN_SDK_PATH}/Bin32
#   $ENV{VULKAN_SDK}/Bin/
#   $ENV{VULKAN_SDK}/Bin32/
# )

# # get all .vert and .frag files in shaders directory
# file(GLOB_RECURSE GLSL_SOURCE_FILES
#   "${PROJECT_SOURCE_DIR}/shaders/*.frag"
#   "${PROJECT_SOURCE_DIR}/shaders/*.vert"
# )

# foreach(GLSL ${GLSL_SOURCE_FILES})
#   get_filename_component(FILE_NAME ${GLSL} NAME)
#   set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
#   add_custom_command(
#     OUTPUT ${SPIRV}
#     COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
#     DEPENDS ${GLSL})
#   list(APPEND SPIRV_BINARY_FILES ${SPIRV})
# endforeach(GLSL)

# add_custom_target(
#     Shaders
#     DEPENDS ${SPIRV_BINARY_FILES}
# )
