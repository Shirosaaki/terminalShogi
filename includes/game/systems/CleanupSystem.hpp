/**==============================================
 *                 CleanupSystem.hpp
 *  divide to better reign
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/ecs/System.hpp"

class CleanupSystem : public ecs::System {
public:
    void update(ecs::Registry& registry, float dt) override;
};
