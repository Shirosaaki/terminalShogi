/**==============================================
 *                 MovementSystem.hpp
 *  can you move here ?
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "../../core/ecs/System.hpp"
#include "../../core/patterns/MoveGeneratorStrategy.hpp"

class MovementSystem : public ecs::System {
public:
    MovementSystem(core::MoveGeneratorStrategy& strategy);

    void update(ecs::Registry& registry, float dt) override;

private:
    core::MoveGeneratorStrategy& m_strategy;
};
