/**==============================================
 *                 Registry.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/core/ecs/Registry.hpp"
#include <algorithm>

namespace ecs {

Registry::Registry()
    : m_lastEntity(INVALID_ENTITY) {}

Registry::~Registry() {}

Entity Registry::createEntity() {
    Entity e = ++m_lastEntity;
    m_alive.push_back(e);
    return e;
}

void Registry::destroyEntity(Entity e) {
    m_toDestroy.push_back(e);
}

const std::vector<Entity>& Registry::aliveEntities() const {
    return m_alive;
}

void Registry::cleanup() {
    if (m_toDestroy.empty()) return;

    for (Entity e : m_toDestroy) {
        // remove from storages
        for (auto& [typeIdx, storage] : m_components) {
            storage->erase(e);
        }

        // remove from alive list
        auto it = std::find(m_alive.begin(), m_alive.end(), e);
        if (it != m_alive.end()) {
            m_alive.erase(it);
        }
    }

    m_toDestroy.clear();
}

} // namespace ecs
