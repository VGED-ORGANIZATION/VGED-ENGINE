#include <iostream>
#include "vged.hpp"
#include "data/entity.hpp"

struct Position {
    float x, y, z;
};

struct Velocity {
    float dx, dy, dz;
};

int main() {
    VGED::Log::init();
    VGED_INFO("Hello!");

    VGED::Engine::Scene scene;
    {
        auto ent = scene.new_entity();
        auto& position = ent.assign<Position>();
    }
    {
        auto ent = scene.new_entity();
        auto [position, velocity] = ent.assign<Position, Velocity>();
    }

    for (auto& [vel] : VGED::Engine::SceneView(scene)) {
        
    }

    return 0;
}