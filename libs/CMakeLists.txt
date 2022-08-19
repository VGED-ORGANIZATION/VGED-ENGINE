set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# set(SHADERC_SKIP_INSTALL ON)
# set(SHADERC_SKIP_TESTS ON)
# set(SHADERC_SKIP_EXAMPLES ON)
# set(SHADERC_ENABLE_COPYRIGHT_CHECK OFF)

# set(SPIRV_HEADERS_SKIP_INSTAL OFF)
# set(SPIRV_HEADERS_SKIP_EXAMPLES ON)

project(libs LANGUAGES C CXX VERSION 1.0.0)

# add_subdirectory(glslang)
# add_subdirectory(cglm)
# add_subdirectory(spirv-tools)
# add_subdirectory(shaderc)
add_subdirectory(glfw)

add_library(${PROJECT_NAME} SHARED
    # $<TARGET_OBJECTS:shaderc>
    $<TARGET_OBJECTS:glfw>
    # $<TARGET_OBJECTS:cglm>
)

target_link_libraries(libs PUBLIC
    #shaderc
    glfw
    #cglm
)