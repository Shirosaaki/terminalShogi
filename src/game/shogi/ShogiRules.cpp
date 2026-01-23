/**==============================================
 *                 ShogiRules.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/shogi/ShogiRules.hpp"

void ShogiRules::update(ecs::Registry&) {
    // TODO: vérifier mat, etc.
}

bool ShogiRules::isGameOver() const {
    return m_over;
}
