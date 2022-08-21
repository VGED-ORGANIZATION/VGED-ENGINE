cmake_minimum_required(VERSION 3.11.0)
project(Vendor VERSION 0.0.1)

if (DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

find_package(Vulkan REQUIRED)
find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/libs/vulkan" REQUIRED)
link_libraries(${Vulkan_LIBRARY})

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(volk)
add_subdirectory(VulkanMemoryAllocator)
add_subdirectory(glslang)
add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_ENABLE_TESTS OFF)
add_subdirectory(shaderc)
add_subdirectory(imgui)