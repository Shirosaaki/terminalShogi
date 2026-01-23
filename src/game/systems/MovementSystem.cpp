/**==============================================
 *                 MovementSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/systems/MovementSystem.hpp"

MovementSystem::MovementSystem(core::MoveGeneratorStrategy& strategy)
    : m_strategy(strategy) {}

void MovementSystem::update(ecs::Registry& registry, float) {
    // Le système ne bouge rien tout seul :
    // il génère les coups possibles et attend une Command pour appliquer un Move.
    auto moves = m_strategy.generateMoves(registry);
    (void)moves;
}
