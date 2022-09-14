#pragma once

#include "../engine/graphics/descriptors.hpp"
#include "../engine/graphics/device.hpp"
#include "../engine/graphics/game_object.hpp"
#include "../engine/graphics/renderer.hpp"
#include "../engine/core/window.hpp"
#include "../engine/graphics/texture.hpp"

// std
#include <memory>
#include <vector>

using namespace VGED::Engine::Graphics;

namespace VGED {
    namespace Editor {
        class EditorApp {
        public:
            static constexpr int WIDTH = 1280;
            static constexpr int HEIGHT = 720;

            EditorApp();
            ~EditorApp();

            EditorApp(const EditorApp &) = delete;
            EditorApp &operator=(const EditorApp &) = delete;

            void run();

        private:
            void loadGameObjects();

            VGED::Engine::Window lveWindow{ WIDTH, HEIGHT, "VGED Engine" };
            Device lveDevice{ lveWindow };
            Renderer lveRenderer{ lveWindow, lveDevice };

            std::unique_ptr<DescriptorPool> globalPool{};
            std::unique_ptr<Texture> texture{};
            GameObject::Map gameObjects;
        };
    }
}
