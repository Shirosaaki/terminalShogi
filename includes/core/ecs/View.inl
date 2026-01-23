#pragma once

namespace ecs {

template<typename... Components>
View<Components...>::View(Registry& reg)
    : m_registry(reg) {}

template<typename... Components>
std::vector<Entity> View<Components...>::entities() const {
    std::vector<Entity> result;
    for (Entity e : m_registry.aliveEntities()) {
        if ((m_registry.hasComponent<Components>(e) && ...)) {
            result.push_back(e);
        }
    }
    return result;
}

} // namespace ecs
