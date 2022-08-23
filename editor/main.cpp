#include "vged.hpp"

#include "first_app.hpp"

int main() {
    VGED::Log::init();

    VGED_INFO("Vulkan Game Engine Dev!");

    lve::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}