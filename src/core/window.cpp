#include "core/window.h"

#include "core/debug.h"

namespace vged {
    
namespace core {

Window::Window(int width, int height, const char *name) 
    : m_width(width),
      m_height(height),
      m_name(name) {
    if (!glfwInit()) {
        THROW("GLFW failed to initialize!");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // todo: change this to true

    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), NULL, NULL);
    if (!m_window) {
        THROW("Failed to create glfw window!");
    }
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

} // namespace core

} // namespace vged
