/**==============================================
 *                 ShogiMoveGenerator.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/shogi/ShogiMoveGenerator.hpp"
#include "../../../includes/game/components/PositionComponent.hpp"
#include "../../../includes/game/components/PieceComponent.hpp"
#include "../../../includes/game/components/BoardCellComponent.hpp"
#include "../../../includes/game/components/TurnComponent.hpp"

namespace {

constexpr int W = 9;
constexpr int H = 9;

bool inBounds(int x, int y) {
    return x >= 0 && x < W && y >= 0 && y < H;
}

ecs::Entity pieceAt(ecs::Registry& reg, int x, int y) {
    for (auto e : reg.aliveEntities()) {
        auto bc = reg.getComponent<BoardCellComponent>(e);
        if (bc && bc->x == x && bc->y == y) {
            return bc->piece;
        }
    }
    return ecs::INVALID_ENTITY;
}

void addStepMoves(std::vector<core::Move>& out,
                  ecs::Registry& reg,
                  int x, int y, int owner,
                  const std::vector<std::pair<int,int>>& dirs) {
    for (auto [dx, dy] : dirs) {
        int nx = x + dx;
        int ny = y + (owner == 0 ? -dy : dy); // orientation inversée
        if (!inBounds(nx, ny)) continue;
        auto target = pieceAt(reg, nx, ny);
        if (target != ecs::INVALID_ENTITY) {
            auto pc = reg.getComponent<PieceComponent>(target);
            if (pc && pc->owner == owner) continue;
        }
        out.push_back(core::Move{x, y, nx, ny, false});
    }
}

void addSlidingMoves(std::vector<core::Move>& out,
                     ecs::Registry& reg,
                     int x, int y, int owner,
                     const std::vector<std::pair<int,int>>& dirs) {
    for (auto [dx, dy] : dirs) {
        int nx = x;
        int ny = y;
        while (true) {
            nx += dx;
            ny += (owner == 0 ? -dy : dy);
            if (!inBounds(nx, ny)) break;
            auto target = pieceAt(reg, nx, ny);
            if (target != ecs::INVALID_ENTITY) {
                auto pc = reg.getComponent<PieceComponent>(target);
                if (pc && pc->owner != owner) {
                    out.push_back(core::Move{x, y, nx, ny, false});
                }
                break;
            }
            out.push_back(core::Move{x, y, nx, ny, false});
        }
    }
}

} // namespace

std::vector<core::Move> ShogiMoveGenerator::generateMoves(ecs::Registry& registry) {
    std::vector<core::Move> moves;

    int currentPlayer = 0;
    for (auto e : registry.aliveEntities()) {
        auto t = registry.getComponent<TurnComponent>(e);
        if (t) {
            currentPlayer = t->currentPlayer;
            break;
        }
    }

    for (auto e : registry.aliveEntities()) {
        auto pos = registry.getComponent<PositionComponent>(e);
        auto pc  = registry.getComponent<PieceComponent>(e);
        if (!pos || !pc) continue;
        if (pc->owner != currentPlayer) continue;

        int x = pos->x;
        int y = pos->y;

        if (pc->name == "King") {
            addStepMoves(moves, registry, x, y, currentPlayer,
                         {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}});
        } else if (pc->name == "Gold") {
            addStepMoves(moves, registry, x, y, currentPlayer,
                         {{0,1},{1,1},{1,0},{0,-1},{-1,0},{-1,1}});
        } else if (pc->name == "Silver") {
            addStepMoves(moves, registry, x, y, currentPlayer,
                         {{0,1},{1,1},{-1,1},{1,-1},{-1,-1}});
        } else if (pc->name == "Pawn") {
            addStepMoves(moves, registry, x, y, currentPlayer,
                         {{0,1}});
        } else if (pc->name == "Knight") {
            int dy = 2;
            int dxs[2] = {-1, 1};
            for (int dx : dxs) {
                int nx = x + dx;
                int ny = y + (currentPlayer == 0 ? -dy : dy);
                if (!inBounds(nx, ny)) continue;
                auto target = pieceAt(registry, nx, ny);
                if (target != ecs::INVALID_ENTITY) {
                    auto tpc = registry.getComponent<PieceComponent>(target);
                    if (tpc && tpc->owner == currentPlayer) continue;
                }
                moves.push_back(core::Move{x, y, nx, ny, false});
            }
        } else if (pc->name == "Lance") {
            addSlidingMoves(moves, registry, x, y, currentPlayer,
                            {{0,1}});
        } else if (pc->name == "Bishop") {
            addSlidingMoves(moves, registry, x, y, currentPlayer,
                            {{1,1},{1,-1},{-1,1},{-1,-1}});
        } else if (pc->name == "Rook") {
            addSlidingMoves(moves, registry, x, y, currentPlayer,
                            {{0,1},{1,0},{0,-1},{-1,0}});
        }
    }

    return moves;
}
