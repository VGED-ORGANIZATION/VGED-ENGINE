add_library(imgui STATIC)

target_sources(imgui
        PRIVATE
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        imgui.cpp

        PRIVATE
        backends/imgui_impl_vulkan.cpp
        backends/imgui_impl_glfw.cpp
        )

target_include_directories(imgui
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/
        PUBLIC backends/
        )

target_link_libraries(imgui volk)