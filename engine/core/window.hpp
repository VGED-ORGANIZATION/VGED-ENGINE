#pragma once

#include "types.hpp"

#include <volk.h>

#define VK_NO_PROTOTYPES
#include <GLFW/glfw3.h>

#include <string>
namespace VGED {
    namespace Engine {
        inline namespace Core {
            class Window {
            public:
                Window(i32 _width, int _height, std::string _name);
                ~Window();

                Window(const Window &) = delete;
                Window &operator=(const Window &) = delete;

                bool should_close() { return glfwWindowShouldClose(glfw_window); }
                VkExtent2D get_extent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
                bool was_resized() { return framebuffer_resized; }
                void reset_resized_flag() { framebuffer_resized = false; }
                GLFWwindow *get_glfw_window() const { return glfw_window; }

                void create_window_surface(VkInstance vk_instance, VkSurfaceKHR *vk_surface);

            private:
                static void framebuffer_resize_callback(GLFWwindow *window, i32 width, i32 height);

                i32 width, height;
                bool framebuffer_resized = false;

                std::string name = {};
                GLFWwindow *glfw_window;
            };
        }
    }
}
