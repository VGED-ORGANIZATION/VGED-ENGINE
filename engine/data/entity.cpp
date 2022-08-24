#include "entity.hpp"


namespace VGED {

namespace Engine
{

Entity Scene::new_entity() {
    return Entity(m_registry, m_registry->create());
}

void Scene::delete_entity(Entity& entity) {
    m_registry->destroy(entity.m_entityID);
}

} // namespace Engine

} // namespace VGED
