/**==============================================
 *                 View.hpp
 *  view
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "Registry.hpp"
#include <vector>

namespace ecs {

template<typename... Components>
class View {
public:
    View(Registry& reg);
    std::vector<Entity> entities() const;

private:
    Registry& m_registry;
};

} // namespace ecs

#include "View.inl"
