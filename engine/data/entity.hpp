#pragma once

#include <entt/single_include/entt/entt.hpp>
#include <memory>

namespace VGED {

namespace Engine {

class Entity;

class Scene {
    friend class Entity;
public:
    Scene() {
        m_registry = std::make_shared<entt::registry>();
    }
    ~Scene() {

    }

    Entity new_entity();
    void delete_entity(Entity& entity);

private:
    std::shared_ptr<entt::registry> m_registry;
};

class Entity {
    friend class Scene;
public:
    template <typename... ComponentTypes>
    decltype(auto) assign() {
        static_assert(sizeof...(ComponentTypes) != 0);
        if constexpr(sizeof...(ComponentTypes) == 1) {
            return (assign_<ComponentTypes...>());
        } else {
            return std::forward_as_tuple<ComponentTypes&...>(assign_<ComponentTypes>()...);
        }
    }

    template <typename... ComponentTypes> 
    decltype(auto) get() {
        return (m_scene->get<ComponentTypes...>(m_entityID));
    }

    template <typename... ComponentTypes>
    void remove() {
        static_assert(sizeof...(ComponentTypes) != 0);
        remove_<ComponentTypes...>();
    }

    template <typename T> 
    bool has() {
        return m_scene->all_of<T>(m_entityID);
    }

private:
    Entity(std::shared_ptr<entt::registry> scene, entt::entity entityID) : m_scene(scene), m_entityID(entityID) {} 

    template <typename T>
    T& assign_() {
        return m_scene->emplace<T>(m_entityID);
    }

    template <typename T, typename... O>
    void remove_() {
        m_scene->remove<T>(m_entityID);
        if constexpr(sizeof...(O) == 0) {
            return;
        } else {
            remove_<O...>(m_entityID);
        }
    }

private:
    std::shared_ptr<entt::registry> m_scene;
    entt::entity m_entityID;

};

template <typename... ComponentTypes>
class SceneView {
public:
    SceneView(Scene& scene) : m_scene(scene) {
        static_assert(sizeof...(ComponentTypes) != 0);  // u need to specify atleast 1 component to loop thru
        m_view = scene.m_registry->view<ComponentTypes...>();
    }

    struct iterator {
        iterator(auto it, auto view) : m_it(it), m_view(view) {}
        bool operator==(const iterator& other) const {
            return m_view.m_it == other.m_view.m_it;
        }
        bool operator!=(const iterator& other) const {
            return m_view.m_it != other.m_view.m_it;
        }
        decltype(auto) operator*() const {
            return *m_it;
        }
        decltype(auto) operator++() {
            m_it++;
            return *this;
        }
        
        entt::basic_view<entt::entity, entt::get_t<ComponentTypes...>, entt::exclude_t<>, void>::iterator m_it;
        entt::basic_view<entt::entity, entt::get_t<ComponentTypes...>, entt::exclude_t<>, void> m_view;
    };

    decltype(auto) begin() const {
        auto begin = m_view.begin();
        return iterator(begin, m_view);
    }
    decltype(auto) end() const {
        return iterator(m_view.end(), m_view);
    }

private:
    std::shared_ptr<Scene> m_scene; 
    entt::basic_view<entt::entity, entt::get_t<ComponentTypes...>, entt::exclude_t<>, void> m_view;
};

}

}