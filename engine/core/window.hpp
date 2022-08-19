#pragma once

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
                Window(int width, int height, const char *name);
                ~Window();

                bool should_close() const { return glfwWindowShouldClose(m_window); }
                void poll_events() const { glfwPollEvents(); }
                const GLFWwindow* window() const { return m_window; }
                int height() const { return m_height; }
                int width() const { return m_width; }
                const std::string& name() const { return m_name; } 
                float aspect_ratio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

                // vulkan specific
                // create_window_surface would be done with instance creation as it requires instance and should not be done here to allow multiple backends

            private:
                GLFWwindow *m_window;
                int         m_width, m_height;
                std::string m_name;
            };

        }
    }
}
