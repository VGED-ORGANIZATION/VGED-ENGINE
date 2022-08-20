#include "vged.hpp"
#include "graphics/device.hpp"
#include "GLFW/glfw3.h"

using namespace VGED::Engine;

int main() {

    VGED::Log::init();
    VGED_INFO("Vulkan Game Engine Dev!");

    Window window(1280, 720, "VGED Engine");
    Device device(&window);

    while(!window.should_close()) {
        glfwPollEvents();
    }

    return 0;
}