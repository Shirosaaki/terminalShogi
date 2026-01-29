/**==============================================
 *                 ShogiLogic.cpp
 *  game logic utilities
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/shogi/ShogiLogic.hpp"
#include "../../../includes/game/components/PositionComponent.hpp"
#include "../../../includes/game/components/BoardCellComponent.hpp"
#include "../../../includes/game/components/TurnComponent.hpp"
#include "../../../includes/game/components/GameStatusComponent.hpp"
#include "../../../includes/ui/InputSystem.hpp"
#include "../../../includes/ui/NcursesRenderer.hpp"
#include <algorithm>
#include <sstream>

namespace shogi {

bool in_promotion_zone(int owner, int y) {
    if (owner == 0) return y >= 0 && y <= 2;
    return y >= 6 && y <= 8;
}

bool can_promote(ecs::Registry& reg, ecs::Entity piece, int toY) {
    auto pc = reg.getComponent<PieceComponent>(piece);
    if (!pc) return false;
    if (pc->name == "King" || pc->name == "Gold") return false;

    auto pos = reg.getComponent<PositionComponent>(piece);
    if (!pos) return false;
    int fromY = pos->y;
    return in_promotion_zone(pc->owner, fromY) || in_promotion_zone(pc->owner, toY);
}

bool mandatory_promote(ecs::Registry& reg, ecs::Entity piece, int toY) {
    auto pc = reg.getComponent<PieceComponent>(piece);
    if (!pc) return false;
    if (pc->name == "Pawn") {
        // Pawn reaching last rank must promote
        if ((pc->owner == 0 && toY == 0) || (pc->owner == 1 && toY == 8)) return true;
    }
    if (pc->name == "Lance") {
        if ((pc->owner == 0 && toY == 0) || (pc->owner == 1 && toY == 8)) return true;
    }
    if (pc->name == "Knight") {
        if (pc->owner == 0 && (toY == 0 || toY == 1)) return true;
        if (pc->owner == 1 && (toY == 7 || toY == 8)) return true;
    }
    return false;
}

void handle_promotion(ecs::Registry& reg, ecs::Entity piece, int toY,
                      void* input, void* renderer) {
    auto pc = reg.getComponent<PieceComponent>(piece);
    if (!pc) return;
    if (!can_promote(reg, piece, toY)) return;

    // New behavior: promote automatically when destination is in promotion zone
    if (in_promotion_zone(pc->owner, toY) || mandatory_promote(reg, piece, toY)) {
        pc->promoted = true;
        return;
    }

    // If not in promotion zone and not mandatory, keep unpromoted (no UI prompt)
}

static ecs::Entity find_cell_entity(ecs::Registry& reg, int x, int y) {
    for (auto e : reg.aliveEntities()) {
        auto bc = reg.getComponent<BoardCellComponent>(e);
        if (bc && bc->x == x && bc->y == y) return e;
    }
    return ecs::INVALID_ENTITY;
}

bool validate_drop(ecs::Registry& reg,
                   char symbol,
                   int owner,
                   int tx, int ty,
                   core::MoveGeneratorStrategy& moveGen,
                   const std::vector<char> hands[2]) {
    // Basic rules: Nifu (no two unpromoted pawns on same file)
    if (std::toupper(symbol) == 'P') {
        for (auto e : reg.aliveEntities()) {
            auto pc = reg.getComponent<PieceComponent>(e);
            auto pos = reg.getComponent<PositionComponent>(e);
            if (!pc || !pos) continue;
            if (pc->owner == owner && pc->name == "Pawn" && !pc->promoted && pos->x == tx) {
                return false; // nifu
            }
        }
    }

    // Invalid drop where piece would have no legal moves
    // Pawn and Lance cannot be dropped on last rank; Knight cannot be dropped on last two ranks
    if (std::toupper(symbol) == 'P') {
        if ((owner == 0 && ty == 0) || (owner == 1 && ty == 8)) return false;
    }
    if (std::toupper(symbol) == 'L') {
        if ((owner == 0 && ty == 0) || (owner == 1 && ty == 8)) return false;
    }
    if (std::toupper(symbol) == 'N') {
        if ((owner == 0 && (ty == 0 || ty == 1)) || (owner == 1 && (ty == 7 || ty == 8))) return false;
    }

    // Simulate drop: create temporary piece and ensure it has at least one legal move
    auto cellEnt = find_cell_entity(reg, tx, ty);
    if (cellEnt == ecs::INVALID_ENTITY) return false;

    // create temporary entity
    auto temp = reg.createEntity();
    auto& ppos = reg.addComponent<PositionComponent>(temp);
    ppos.x = tx; ppos.y = ty;
    auto& ppc = reg.addComponent<PieceComponent>(temp);
    auto symToName = [](char s) {
        switch (std::toupper(s)) {
            case 'P': return std::string("Pawn");
            case 'K': return std::string("King");
            case 'L': return std::string("Lance");
            case 'N': return std::string("Knight");
            case 'S': return std::string("Silver");
            case 'G': return std::string("Gold");
            case 'B': return std::string("Bishop");
            case 'R': return std::string("Rook");
            default:  return std::string("Pawn");
        }
    };
    ppc.name = symToName(symbol);
    ppc.symbol = symbol;
    ppc.owner = owner;
    ppc.promoted = false;

    // link to board cell
    auto bc = reg.getComponent<BoardCellComponent>(cellEnt);
    auto oldPiece = bc->piece;
    bc->piece = temp;

    // set turn to owner and generate moves
    int oldTurn = 0;
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) { oldTurn = t->currentPlayer; t->currentPlayer = owner; break; }
    }
    auto moves = moveGen.generateMoves(reg);
    bool hasMove = false;
    for (auto &m : moves) {
        if (m.fromX == tx && m.fromY == ty) { hasMove = true; break; }
    }

    // restore turn
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) { t->currentPlayer = oldTurn; break; }
    }

    // remove temp
    bc->piece = oldPiece;
    reg.destroyEntity(temp);

    if (!hasMove) return false;

    // Uchifuzume: Pawn drops that immediately mate are illegal
    if (std::toupper(symbol) == 'P') {
        // prepare hands copy where one pawn is removed
        std::vector<char> handsCopy0 = hands[0];
        std::vector<char> handsCopy1 = hands[1];
        std::vector<char> handsCopyArr[2] { handsCopy0, handsCopy1 };
        bool removed = false;
        for (auto &c : handsCopyArr[owner]) {
            if (std::toupper(c) == 'P') { c = 0; removed = true; break; }
        }
        if (!removed) {
            // should not happen: pawn not in hand
        } else {
            if (is_checkmate(reg, 1 - owner, moveGen, handsCopyArr)) return false;
        }
    }

    return true;
}

bool is_in_check(ecs::Registry& reg, int player, core::MoveGeneratorStrategy& moveGen) {
    // find king
    int kx=-1, ky=-1;
    for (auto e : reg.aliveEntities()) {
        auto pc = reg.getComponent<PieceComponent>(e);
        auto pos = reg.getComponent<PositionComponent>(e);
        if (!pc || !pos) continue;
        if (pc->name == "King" && pc->owner == player) { kx = pos->x; ky = pos->y; break; }
    }
    if (kx < 0) return false;

    // set turn to opponent and generate moves
    int opponent = 1 - player;
    int oldTurn = 0;
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) { oldTurn = t->currentPlayer; t->currentPlayer = opponent; break; }
    }
    auto moves = moveGen.generateMoves(reg);
    // restore turn
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) { t->currentPlayer = oldTurn; break; }
    }

    for (auto &m : moves) {
        if (m.toX == kx && m.toY == ky) return true;
    }
    return false;
}

// Helper to simulate a move in-place and undo it
struct SimSnapshot {
    ecs::Entity piece;
    int fromX, fromY, toX, toY;
    ecs::Entity target;
    int targetPrevX, targetPrevY;
    ecs::Entity bcFrom, bcTo;
    ecs::Entity targetEntityAtTo;
};

static SimSnapshot simulate_move(ecs::Registry& reg, int fx, int fy, int tx, int ty) {
    SimSnapshot s{};
    s.fromX = fx; s.fromY = fy; s.toX = tx; s.toY = ty;
    s.piece = ecs::INVALID_ENTITY;
    s.target = ecs::INVALID_ENTITY;
    s.bcFrom = ecs::INVALID_ENTITY; s.bcTo = ecs::INVALID_ENTITY;

    for (auto e : reg.aliveEntities()) {
        auto bc = reg.getComponent<BoardCellComponent>(e);
        if (!bc) continue;
        if (bc->x == fx && bc->y == fy) s.bcFrom = e;
        if (bc->x == tx && bc->y == ty) s.bcTo = e;
    }

    if (s.bcFrom == ecs::INVALID_ENTITY || s.bcTo == ecs::INVALID_ENTITY) return s;

    auto bcFromComp = reg.getComponent<BoardCellComponent>(s.bcFrom);
    auto bcToComp = reg.getComponent<BoardCellComponent>(s.bcTo);
    s.piece = bcFromComp->piece;
    s.target = bcToComp->piece;

    if (s.piece != ecs::INVALID_ENTITY) {
        auto ppos = reg.getComponent<PositionComponent>(s.piece);
        if (ppos) { ppos->x = tx; ppos->y = ty; }
    }

    // move board links
    bcFromComp->piece = ecs::INVALID_ENTITY;
    bcToComp->piece = s.piece;

    if (s.target != ecs::INVALID_ENTITY) {
        auto tpos = reg.getComponent<PositionComponent>(s.target);
        if (tpos) { s.targetPrevX = tpos->x; s.targetPrevY = tpos->y; tpos->x = -1; tpos->y = -1; }
    }

    return s;
}

static void undo_simulate_move(ecs::Registry& reg, SimSnapshot const& s) {
    if (s.bcFrom != ecs::INVALID_ENTITY && s.bcTo != ecs::INVALID_ENTITY) {
        auto bcFromComp = reg.getComponent<BoardCellComponent>(s.bcFrom);
        auto bcToComp = reg.getComponent<BoardCellComponent>(s.bcTo);
        bcFromComp->piece = s.piece;
        bcToComp->piece = s.target;
    }
    if (s.piece != ecs::INVALID_ENTITY) {
        auto ppos = reg.getComponent<PositionComponent>(s.piece);
        if (ppos) { ppos->x = s.fromX; ppos->y = s.fromY; }
    }
    if (s.target != ecs::INVALID_ENTITY) {
        auto tpos = reg.getComponent<PositionComponent>(s.target);
        if (tpos) { tpos->x = s.targetPrevX; tpos->y = s.targetPrevY; }
    }
}

bool is_checkmate(ecs::Registry& reg, int player, core::MoveGeneratorStrategy& moveGen,
                  const std::vector<char> hands[2]) {
    if (!is_in_check(reg, player, moveGen)) return false;

    // save turn
    int oldTurn = 0;
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) { oldTurn = t->currentPlayer; break; }
    }

    // Generate player's moves and test each
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) { t->currentPlayer = player; break; }
    }
    auto moves = moveGen.generateMoves(reg);
    for (auto &m : moves) {
        // simulate move
        auto snap = simulate_move(reg, m.fromX, m.fromY, m.toX, m.toY);
        bool stillInCheck = is_in_check(reg, player, moveGen);
        undo_simulate_move(reg, snap);
        if (!stillInCheck) {
            // restore turn
            for (auto e : reg.aliveEntities()) {
                auto t = reg.getComponent<TurnComponent>(e);
                if (t) { t->currentPlayer = oldTurn; break; }
            }
            return false;
        }
    }

    // Consider drops from hand
    for (char c : hands[player]) {
        if (c == 0) continue;
        for (auto cell : reg.aliveEntities()) {
            auto bc = reg.getComponent<BoardCellComponent>(cell);
            if (!bc) continue;
            if (bc->piece != ecs::INVALID_ENTITY) continue;
            if (!validate_drop(reg, c, player, bc->x, bc->y, moveGen, hands)) continue;

            // perform drop
            auto temp = reg.createEntity();
            auto& ppos = reg.addComponent<PositionComponent>(temp);
            ppos.x = bc->x; ppos.y = bc->y;
            auto& ppc = reg.addComponent<PieceComponent>(temp);
            ppc.name = std::string(1, std::toupper(c)); // placeholder name mapping handled elsewhere
            // better map symbol to name
            auto symToName = [](char s) {
                switch (std::toupper(s)) {
                    case 'P': return std::string("Pawn");
                    case 'K': return std::string("King");
                    case 'L': return std::string("Lance");
                    case 'N': return std::string("Knight");
                    case 'S': return std::string("Silver");
                    case 'G': return std::string("Gold");
                    case 'B': return std::string("Bishop");
                    case 'R': return std::string("Rook");
                    default:  return std::string("Pawn");
                }
            };
            ppc.name = symToName(c);
            ppc.symbol = c;
            ppc.owner = player;
            ppc.promoted = false;
            bc->piece = temp;

            bool stillInCheck = is_in_check(reg, player, moveGen);

            // undo drop
            bc->piece = ecs::INVALID_ENTITY;
            reg.destroyEntity(temp);

            if (!stillInCheck) {
                for (auto e : reg.aliveEntities()) {
                    auto t = reg.getComponent<TurnComponent>(e);
                    if (t) { t->currentPlayer = oldTurn; break; }
                }
                return false;
            }
        }
    }

    // restore turn
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) { t->currentPlayer = oldTurn; break; }
    }

    return true;
}

} // namespace shogi
