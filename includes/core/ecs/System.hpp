/**==============================================
 *                 System.hpp
 *  system
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "Registry.hpp"

namespace ecs {

class System {
public:
    virtual ~System() = default;
    virtual void update(Registry& registry, float dt) = 0;
};

} // namespace ecs

