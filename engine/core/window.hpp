#pragma once

#include "types.hpp"

#include <volk.h>

#define VK_NO_PROTOTYPES
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

namespace VGED {
    namespace Engine {
        inline namespace Core {
            class Window {
            public:
                Window(const int &_width, const int &_height, const std::string &_name);
                ~Window();

                Window(const Window &) = delete;
                Window &operator=(const Window &) = delete;

                bool should_close() { return glfwWindowShouldClose(window); }
                VkExtent2D get_extent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
                bool was_window_resized() { return framebuffer_resized; }
                void reset_window_resized_flag() { framebuffer_resized = false; }
                GLFWwindow *get_GLFWwindow() const { return window; }
                void create_window_surface(VkInstance instance, VkSurfaceKHR *surface);

                i32 get_width() { return width; }
                i32 get_height() { return height; }

            private:
                static void framebuffer_resize_callback(GLFWwindow *window, int width, int height);

                i32 width, height;
                bool framebuffer_resized = false;
                std::string name;
                GLFWwindow *window;
            };
        }
    }
}
