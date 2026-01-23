/**==============================================
 *                 RulesSystem.hpp
 *  can you really do that ?
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "../../core/ecs/System.hpp"
#include "../../core/patterns/GameState.hpp"

class RulesSystem : public ecs::System {
public:
    RulesSystem(core::GameState& state);

    void update(ecs::Registry& registry, float dt) override;

private:
    core::GameState& m_state;
};
