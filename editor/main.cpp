#include <iostream>

#include "../engine/graphics/device.hpp"
#include "GLFW/glfw3.h"
using namespace VGED::Engine;

int main() {
    std::cout << "Vulkan Game Engine Dev!\n";

    Window window(1280, 720, "VGED Engine");
    Device device(&window);

    while(!window.should_close()) {
        glfwPollEvents();
    }

    return 0;
}