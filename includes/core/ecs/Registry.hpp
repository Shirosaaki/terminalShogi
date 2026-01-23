/**==============================================
 *                 Registry.hpp
 *  registry
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "Entity.hpp"
#include "Component.hpp"

#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>

namespace ecs {

class Registry {
public:
    Registry();
    ~Registry();

    Entity createEntity();
    void destroyEntity(Entity e);

    template<typename C, typename... Args>
    C& addComponent(Entity e, Args&&... args);

    template<typename C>
    C* getComponent(Entity e);

    template<typename C>
    bool hasComponent(Entity e) const;

    template<typename C>
    void removeComponent(Entity e);

    const std::vector<Entity>& aliveEntities() const;

    void cleanup();

private:
    struct IStorage {
        virtual ~IStorage() = default;
        virtual void erase(Entity e) = 0;
    };

    template<typename C>
    struct Storage : IStorage {
        std::unordered_map<Entity, C> data;
        void erase(Entity e) override { data.erase(e); }
    };

    template<typename C>
    Storage<C>* getStorage();

    template<typename C>
    Storage<C>& getOrCreateStorage();

private:
    Entity m_lastEntity;
    std::vector<Entity> m_alive;
    std::vector<Entity> m_toDestroy;

    std::unordered_map<std::type_index, std::unique_ptr<IStorage>> m_components;
};

} // namespace ecs

#include "Registry.inl"
