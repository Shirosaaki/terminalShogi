#pragma once

namespace ecs {

template<typename C>
typename Registry::Storage<C>* Registry::getStorage() {
    auto it = m_components.find(std::type_index(typeid(C)));
    if (it == m_components.end()) return nullptr;
    return static_cast<Storage<C>*>(it->second.get());
}

template<typename C>
typename Registry::Storage<C>& Registry::getOrCreateStorage() {
    auto typeIdx = std::type_index(typeid(C));
    auto it = m_components.find(typeIdx);
    if (it == m_components.end()) {
        auto ptr = std::make_unique<Storage<C>>();
        auto raw = ptr.get();
        m_components.emplace(typeIdx, std::move(ptr));
        return *raw;
    }
    return *static_cast<Storage<C>*>(it->second.get());
}

template<typename C, typename... Args>
C& Registry::addComponent(Entity e, Args&&... args) {
    auto& storage = getOrCreateStorage<C>();
    auto it = storage.data.find(e);
    if (it != storage.data.end()) {
        return it->second;
    }
    C comp(std::forward<Args>(args)...);
    auto [insertIt, _] = storage.data.emplace(e, std::move(comp));
    return insertIt->second;
}

template<typename C>
C* Registry::getComponent(Entity e) {
    auto* storage = getStorage<C>();
    if (!storage) return nullptr;
    auto it = storage->data.find(e);
    if (it == storage->data.end()) return nullptr;
    return &it->second;
}

template<typename C>
bool Registry::hasComponent(Entity e) const {
    auto it = m_components.find(std::type_index(typeid(C)));
    if (it == m_components.end()) return false;
    auto* storage = static_cast<Storage<C>*>(it->second.get());
    return storage->data.find(e) != storage->data.end();
}

template<typename C>
void Registry::removeComponent(Entity e) {
    auto* storage = getStorage<C>();
    if (!storage) return;
    storage->data.erase(e);
}

} // namespace ecs
