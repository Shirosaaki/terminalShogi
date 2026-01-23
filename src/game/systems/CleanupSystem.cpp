/**==============================================
 *                 CleanupSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/game/systems/CleanupSystem.hpp"

void CleanupSystem::update(ecs::Registry& registry, float) {
    registry.cleanup();
}
