#include <iostream>

#include "first_app.hpp"

int main() {
    std::cout << "Vulkan Game Engine Dev!\n";

    lve::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}