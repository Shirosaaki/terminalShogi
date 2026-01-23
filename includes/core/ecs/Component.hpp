/**==============================================
 *                 Component.hpp
 *  component
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include <cstddef>

namespace ecs {

struct IComponent {
    virtual ~IComponent() = default;
};

using ComponentTypeId = std::size_t;

ComponentTypeId nextComponentTypeId();

template<typename T>
ComponentTypeId getComponentTypeId() {
    static ComponentTypeId id = nextComponentTypeId();
    return id;
}

} // namespace ecs
