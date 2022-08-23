#include "window.hpp"

// std
#include <stdexcept>

namespace VGED {
	namespace Engine {
		inline namespace Core {
			Window::Window(int _width, int _height, std::string _name) : width{ _width }, height{ _height }, name{ _name } {
				glfwInit();
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

				glfw_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
				glfwSetWindowUserPointer(glfw_window, this);
				glfwSetFramebufferSizeCallback(glfw_window, framebuffer_resize_callback);
			}

			Window::~Window() {
				glfwDestroyWindow(glfw_window);
				glfwTerminate();
			}

			void Window::create_window_surface(VkInstance vk_instance, VkSurfaceKHR *vk_surface) {
				if (glfwCreateWindowSurface(vk_instance, glfw_window, nullptr, vk_surface) != VK_SUCCESS) {
					throw std::runtime_error("failed to craete window surface");
				}
			}

			void Window::framebuffer_resize_callback(GLFWwindow *glfw_window, i32 width, i32 height) {
				auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
				window->framebuffer_resized = true;
				window->width = width;
				window->height = height;
			}
		}
	}
}
