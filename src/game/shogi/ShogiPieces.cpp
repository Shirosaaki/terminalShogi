/**==============================================
 *                 ShogiPieces.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/shogi/ShogiPieces.hpp"
#include "../../../includes/core/ecs/Component.hpp"
#include "../../../includes/game/components/PieceComponent.hpp"
#include "../../../includes/game/components/BoardCellComponent.hpp"
#include "../../../includes/game/components/TurnComponent.hpp"
#include "../../../includes/game/components/GameStatusComponent.hpp"
#include "../../../includes/game/components/PositionComponent.hpp"

namespace shogi {

static constexpr int BOARD_W = 9;
static constexpr int BOARD_H = 9;

void createBoard(ecs::Registry& registry) {
    for (int y = 0; y < BOARD_H; ++y) {
        for (int x = 0; x < BOARD_W; ++x) {
            auto cell = registry.createEntity();
            auto& pos = registry.addComponent<PositionComponent>(cell);
            pos.x = x;
            pos.y = y;

            auto& bc = registry.addComponent<BoardCellComponent>(cell);
            bc.x = x;
            bc.y = y;
            bc.piece = ecs::INVALID_ENTITY;
        }
    }

    auto turn = registry.createEntity();
    registry.addComponent<TurnComponent>(turn).currentPlayer = 0;

    auto status = registry.createEntity();
    registry.addComponent<GameStatusComponent>(status);
}

static ecs::Entity createPiece(ecs::Registry& reg, int x, int y,
                               const std::string& name, char symbol, int owner) {
    auto e = reg.createEntity();
    auto& pos = reg.addComponent<PositionComponent>(e);
    pos.x = x;
    pos.y = y;

    auto& pc = reg.addComponent<PieceComponent>(e);
    pc.name = name;
    pc.symbol = symbol;
    pc.owner = owner;

    // lier à la case
    for (auto cell : reg.aliveEntities()) {
        auto bc = reg.getComponent<BoardCellComponent>(cell);
        if (bc && bc->x == x && bc->y == y) {
            bc->piece = e;
            break;
        }
    }

    return e;
}

void createPieces(ecs::Registry& registry) {
    createBoard(registry);

    // Joueur 0 (en bas)
    // Rang 0
    createPiece(registry, 0, 8, "Lance",  'L', 0);
    createPiece(registry, 1, 8, "Knight", 'N', 0);
    createPiece(registry, 2, 8, "Silver", 'S', 0);
    createPiece(registry, 3, 8, "Gold",   'G', 0);
    createPiece(registry, 4, 8, "King",   'K', 0);
    createPiece(registry, 5, 8, "Gold",   'G', 0);
    createPiece(registry, 6, 8, "Silver", 'S', 0);
    createPiece(registry, 7, 8, "Knight", 'N', 0);
    createPiece(registry, 8, 8, "Lance",  'L', 0);

    // Rang 1
    createPiece(registry, 1, 7, "Bishop", 'B', 0);
    createPiece(registry, 7, 7, "Rook",   'R', 0);

    // Rang 2 (pions)
    for (int x = 0; x < 9; ++x) {
        createPiece(registry, x, 6, "Pawn", 'P', 0);
    }

    // Joueur 1 (en haut) — symétrique
    // Rang 0
    createPiece(registry, 0, 0, "Lance",  'l', 1);
    createPiece(registry, 1, 0, "Knight", 'n', 1);
    createPiece(registry, 2, 0, "Silver", 's', 1);
    createPiece(registry, 3, 0, "Gold",   'g', 1);
    createPiece(registry, 4, 0, "King",   'k', 1);
    createPiece(registry, 5, 0, "Gold",   'g', 1);
    createPiece(registry, 6, 0, "Silver", 's', 1);
    createPiece(registry, 7, 0, "Knight", 'n', 1);
    createPiece(registry, 8, 0, "Lance",  'l', 1);

    // Rang 1
    createPiece(registry, 7, 1, "Bishop", 'b', 1);
    createPiece(registry, 1, 1, "Rook",   'r', 1);

    // Rang 2 (pions)
    for (int x = 0; x < 9; ++x) {
        createPiece(registry, x, 2, "Pawn", 'p', 1);
    }
}

} // namespace shogi
