/**==============================================
 *                 ShogiRules.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/shogi/ShogiRules.hpp"
#include "../../../includes/game/components/GameStatusComponent.hpp"
#include "../../../includes/game/components/PieceComponent.hpp"

void ShogiRules::update(ecs::Registry& registry) {
    bool king0 = false;
    bool king1 = false;

    for (auto e : registry.aliveEntities()) {
        auto pc = registry.getComponent<PieceComponent>(e);
        if (!pc) continue;
        if (pc->name == "King") {
            if (pc->owner == 0) king0 = true;
            else king1 = true;
        }
    }

    if (!king0 || !king1) {
        m_over = true;
        int winner = king0 ? 0 : 1;

        for (auto e : registry.aliveEntities()) {
            auto gs = registry.getComponent<GameStatusComponent>(e);
            if (gs) {
                gs->gameOver = true;
                gs->winner = winner;
            }
        }
    }
}

bool ShogiRules::isGameOver() const {
    return m_over;
}
