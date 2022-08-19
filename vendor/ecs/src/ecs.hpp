#pragma once

#include <iostream>

#ifndef NDEBUG 
#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " (" << __LINE__ << "): " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)

#else
#define ASSERT(condition, message);

#endif

#define THROW(message) std::cerr << "Error in file " << __FILE__ << "(" << __LINE__ << "): " << message << std::endl; std::terminate()

#include <bitset>
#include <vector>
#include <queue>
#include <unordered_set>

namespace ecs {

#define MAX_COMPONENTS 32
#define MAX_ENTITIES 100000

using EntityID = unsigned long long;
using ComponentID = uint32_t;
using ComponentMask = std::bitset<MAX_COMPONENTS>;

class BaseComponentPool {
public:
    virtual void* get(size_t index) = 0;
    virtual void* construct(size_t index) = 0;
    virtual void destroy(size_t index) = 0;
private:

};
template <typename T>
class ComponentPool : public BaseComponentPool {
public:
    ComponentPool() {
        m_componentSize = sizeof(T);
        p_data = new char[m_componentSize * MAX_ENTITIES];
    }
    ~ComponentPool() {
        delete[] p_data;
    }
    void* get(size_t index) override {
        return p_data + index * m_componentSize;
    }
    void* construct(size_t index) override {
        return new (get(index)) T();
    }
    void destroy(size_t index) override {
        ((T*)(get(index)))->~T();
    }
private:
    char* p_data{nullptr};
    size_t m_componentSize{0};
};

class Scene {
    struct EntityDescription {
        EntityID id;
        ComponentMask mask;
        bool isValid;
    };
public:
    Scene() {
        entities.reserve(MAX_ENTITIES);
        for (int i = 0; i < MAX_ENTITIES; i++) {
            availableIDs.push(static_cast<EntityID>(i));
        }
    }
    ~Scene() {
        for (auto& componentPool: componentPools) {
            delete componentPool;
        }
    }

    template <typename T>
    ComponentID getComponentID() {
        static ComponentID s_ComponentID = s_ComponentCounter++;
        static bool once = [this]() {
            this->registeredComponents.insert(s_ComponentID);
            if (this->componentPools.size() <= s_ComponentID) {
                this->componentPools.resize(s_ComponentID + 1, nullptr);
            }
            if (this->componentPools[s_ComponentID] == nullptr) {
                this->componentPools[s_ComponentID] = new ComponentPool<T>();
            }
            return true;
        }();
        return s_ComponentID;
    }

    const EntityID newEntity() {
        EntityID entityID = availableIDs.front();
        availableIDs.pop();
        if (entities.size() <= entityID)
        {
            entities.push_back({entityID, ComponentMask(), true});
        }
        ASSERT(entities[entityID].isValid == true, "this entity should not be in the available queue");
        return entityID;
    }
    void deleteEntity(const EntityID entityID) {
        ASSERT(entities[entityID].isValid == true, "cannot delete already deleted entity");
        for (int componentID=0; componentID<MAX_COMPONENTS; componentID++)
        {
            if (!entities[entityID].mask.test(componentID)) continue;
            componentPools[componentID]->destroy(entityID);
        }
        entities[entityID].isValid = false;
        entities[entityID].mask.reset();
        availableIDs.push(entityID);
    }
    // void* assign(EntityID entityID, ComponentID componentID) {
    //     ASSERT(registeredComponents.find(componentID) != registeredComponents.end(), "component id provided not a registered component, use component ids only given by getComponentID<T>();");
    //     ASSERT(entities[entityID].mask.test(componentID) == false, "entity already has component");
    //     void* p_component = componentPools[componentID]->construct(entityID);
    //     entities[entityID].mask.set(componentID);
    //     return p_component;
    // }
    template <typename... ComponentTypes>
    decltype(auto) assign(EntityID entityID) {
        static_assert(sizeof...(ComponentTypes) != 0);   // suggested by zilverblade
        if constexpr(sizeof...(ComponentTypes) == 1) {
            return (assign_<ComponentTypes...>(entityID));
        } else {
            return std::forward_as_tuple<ComponentTypes&...>(assign_<ComponentTypes>(entityID)...);
        }
    }
    template <typename... ComponentTypes>
    decltype(auto) get(EntityID entityID) {
        static_assert(sizeof...(ComponentTypes) != 0);   // suggested by zilverblade
        if constexpr(sizeof...(ComponentTypes) == 1) {
            return (get_<ComponentTypes...>(entityID));
        } else {
            return std::forward_as_tuple<ComponentTypes&...>(get_<ComponentTypes>(entityID)...);
        }
    }
    template <typename... ComponentTypes>
    void remove(EntityID entityID) {
        static_assert(sizeof...(ComponentTypes) != 0);   // suggested by zilverblade
        remove_<ComponentTypes...>(entityID);                
    }
    template <typename T>
    bool has(EntityID entityID) {
        ASSERT(isValid(entityID) == true, "entity does not exist");
        return entities[entityID].mask.test(getComponentID<T>());
    }
    
    bool isValid(EntityID entityID) {
        ASSERT(entities.size() > entityID, "entity doesnt exist");
        return entities[entityID].isValid;
    }
    std::vector<EntityDescription> entities;

private:
    template <typename T, typename... O>
    void remove_(EntityID entityID) {
        ComponentID componentID = getComponentID<T>();
        ASSERT(isValid(entityID) == true, "entity does not exist");
        ASSERT(entities[entityID].mask.test(componentID) == true, "cannot remove component from entity which doesnt contain it in the first place");
        componentPools[componentID]->destroy(entityID);
        entities[entityID].mask.reset(componentID);
        if constexpr(sizeof...(O) == 0) {
            return;
        } else {
            remove_<O...>(entityID);
        }
    }
    template <typename T>
    T& get_(EntityID entityID) {
        ASSERT(entities[entityID].isValid == true, "entity does not exist");
        ASSERT(has<T>(entityID) == true, "entity does not contain requested component");
        ComponentID componentID = getComponentID<T>();
        return *((T*)(componentPools[componentID]->get(entityID)));
    }
    template <typename T>
    T& assign_(EntityID entityID) {
        ASSERT(entities[entityID].isValid == true, "entity does not exist");
        ComponentID componentID = getComponentID<T>();
        ASSERT(entities[entityID].mask.test(componentID) == false, "entity already has component");
        T* p_component = (T*)(componentPools[componentID]->construct(static_cast<size_t>(entityID)));
        entities[entityID].mask.set(componentID);
        return *p_component;
    }

private:
    std::queue<EntityID> availableIDs;
    std::vector<BaseComponentPool*> componentPools;
    ComponentID s_ComponentCounter = 0;
    std::unordered_set<ComponentID> registeredComponents;
};

template <typename... ComponentTypes>
struct SceneView {
    SceneView(Scene &scene) : scene(scene) {
        if constexpr(sizeof...(ComponentTypes) == 0) {
            all = true;
        } else {
            ComponentID componentIDs[] = {scene.getComponentID<ComponentTypes>()...};
            for (int i = 0; i < sizeof...(ComponentTypes); i++) {
                componentMask.set(componentIDs[i]);
            }
        }
    }
    struct Iterator {
        Iterator(Scene &scene, EntityID index, ComponentMask mask, bool all) :
            scene(scene), index(index), mask(mask), all(all) {

        }

        bool isValidIndex() {
            return scene.isValid(scene.entities[index].id) && (all || mask == (mask & scene.entities[index].mask));
        }
        decltype(auto) operator*() const {
            if constexpr(sizeof...(ComponentTypes) != 0) {
                return std::forward_as_tuple<EntityID&, ComponentTypes&...>(scene.entities[index].id, scene.get<ComponentTypes>(scene.entities[index].id)...);
            } else {
                return (scene.entities[index].id);
            }
        }
        bool operator==(const Iterator &other) const {
            return index == other.index || index == scene.entities.size();
        }
        bool operator!=(const Iterator &other) const {
            return index != other.index && index != scene.entities.size();
        }
        decltype(auto) operator++() {
            do  {
                index++;
            } while(index < scene.entities.size() && !isValidIndex());
            return *this;
        }
        EntityID index;
        Scene &scene;
        ComponentMask mask;
        bool all = false;
    };

    const Iterator begin() const {
        EntityID firstIndex = 0;
        while (firstIndex < scene.entities.size() && (componentMask != (componentMask & scene.entities[firstIndex].mask) || !scene.isValid(scene.entities[firstIndex].id))) {
            firstIndex++;
        } 
        
        return Iterator(scene, firstIndex, componentMask, all);
    }
    
    const Iterator end() const  {
        return Iterator(scene, EntityID{scene.entities.size()}, componentMask, all);
    }
    Scene &scene;
    bool all = false;
    ComponentMask componentMask;
};

} // namespace ecs
