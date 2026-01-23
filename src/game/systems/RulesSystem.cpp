/**==============================================
 *                 RulesSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/systems/RulesSystem.hpp"

RulesSystem::RulesSystem(core::GameState& state)
    : m_state(state) {}

void RulesSystem::update(ecs::Registry& registry, float) {
    m_state.update(registry);
}
