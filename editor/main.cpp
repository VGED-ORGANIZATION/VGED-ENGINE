#include "vged.hpp"

#include "editor_app.hpp"

using namespace VGED;

int main() {
	VGED::Engine::Core::Log::init();

	VGED_INFO("Vulkan Game Engine Dev!");

	VGED::Editor::EditorApp app{};

	try {
		app.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return 0;
}