/**==============================================
 *                 InputSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/systems/InputSystem.hpp"
#include "../../../includes/ui/InputSystem.hpp"
#include "../../../includes/game/components/PositionComponent.hpp"
#include "../../../includes/game/components/PieceComponent.hpp"
#include "../../../includes/game/components/BoardCellComponent.hpp"
#include "../../../includes/game/components/TurnComponent.hpp"
#include "../../../includes/game/components/GameStatusComponent.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>
#include <fstream>
#include <cstdio>
#include <ncurses.h>
#include <sstream>
#include <unistd.h>
#include "../../../includes/core/utils/SignalHandler.hpp"

InputSystem::InputSystem(core::MoveGeneratorStrategy& moveGen,
                         ui::NcursesRenderer& renderer,
                         ui::InputSystem& input,
                         int localPlayer,
                         pid_t opponentPid)
    : m_moveGen(moveGen), m_renderer(renderer), m_input(input), m_localPlayer(localPlayer), m_opponentPid(opponentPid) {}

bool InputSystem::readCoords(int& x, int& y) {
    std::string line = m_input.readLineBlocking();
    std::stringstream ss(line);
    int ux, uy;
    if ((ss >> ux >> uy).fail()) return false;
    // Convert from 1-based (user) to 0-based (internal)
    x = ux - 1;
    y = uy - 1;
    return (x >= 0 && y >= 0);
}

ecs::Entity InputSystem::pieceAt(ecs::Registry& reg, int x, int y) {
    for (auto e : reg.aliveEntities()) {
        auto pos = reg.getComponent<PositionComponent>(e);
        auto pc  = reg.getComponent<PieceComponent>(e);
        if (!pos || !pc) continue;
        if (pos->x == x && pos->y == y)
            return e;
    }
    return ecs::INVALID_ENTITY;
}

void InputSystem::applyMove(ecs::Registry& reg, const core::Move& m) {
    auto piece = pieceAt(reg, m.fromX, m.fromY);
    if (piece == ecs::INVALID_ENTITY) return;

    // remove captured piece
    auto target = pieceAt(reg, m.toX, m.toY);
    if (target != ecs::INVALID_ENTITY) {
        reg.destroyEntity(target);
    }

    // update position
    auto pos = reg.getComponent<PositionComponent>(piece);
    if (pos) {
        pos->x = m.toX;
        pos->y = m.toY;
    }

    // update board cells
    for (auto e : reg.aliveEntities()) {
        auto bc = reg.getComponent<BoardCellComponent>(e);
        if (!bc) continue;

        if (bc->x == m.fromX && bc->y == m.fromY)
            bc->piece = ecs::INVALID_ENTITY;

        if (bc->x == m.toX && bc->y == m.toY)
            bc->piece = piece;
    }
}

void InputSystem::update(ecs::Registry& reg, float) {
    // Check if game over
    for (auto e : reg.aliveEntities()) {
        auto gs = reg.getComponent<GameStatusComponent>(e);
        if (gs && gs->gameOver)
            return;
    }

    // Get current player
    int currentPlayer = 0;
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) {
            currentPlayer = t->currentPlayer;
            break;
        }
    }

    // Only allow input if it's this process's turn
    if (currentPlayer != m_localPlayer) {
        // Player 1 (localPlayer==0) should NOT wait for a signal on the first turn
        static bool firstTurn = true;
        if (firstTurn && m_localPlayer == 0) {
            firstTurn = false;
            // Let Player 1 play immediately
        } else {
            m_renderer.drawText(0, 20, "Your enemy is playing... (waiting for their move)");
            m_renderer.refreshScreen();

            // wait until a SIGUSR1 from opponent is consumed
            while (!core::SignalHandler::consumeUserSignal()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            // apply the move published by the opponent
            pid_t sender = core::SignalHandler::lastUserSender();
            if (sender != 0) {
                std::string path = "/tmp/terminalShogi_move_" + std::to_string(sender);
                std::ifstream in(path);
                if (in) {
                    core::Move m;
                    if (in >> m.fromX >> m.fromY >> m.toX >> m.toY) {
                        applyMove(reg, m);

                        // ensure turn is set to local player (the one who was waiting)
                        for (auto e : reg.aliveEntities()) {
                            auto t = reg.getComponent<TurnComponent>(e);
                            if (t) {
                                t->currentPlayer = m_localPlayer;
                                break;
                            }
                        }
                    }
                    in.close();
                    std::remove(path.c_str());
                }
            }

            return;
        }
    }

    // Ask for input
    m_renderer.drawText(0, 20, "Enter x y : ");
    m_renderer.refreshScreen();

    int x, y;
    while (!readCoords(x, y)) {
        m_renderer.drawText(0, 21, "Invalid input. Enter x y : ");
        m_renderer.refreshScreen();
    }

    // Check piece
    ecs::Entity piece = pieceAt(reg, x, y);
    std::stringstream dbg;
    dbg << "DEBUG: Input (" << (x+1) << "," << (y+1) << ") ";
    if (piece == ecs::INVALID_ENTITY) {
        dbg << "No piece found.";
        m_renderer.drawText(0, 22, dbg.str());
        m_renderer.refreshScreen();
        return;
    }

    auto pc = reg.getComponent<PieceComponent>(piece);
    if (!pc) {
        dbg << "Entity has no PieceComponent.";
        m_renderer.drawText(0, 22, dbg.str());
        m_renderer.refreshScreen();
        return;
    }
    dbg << "owner=" << pc->owner << ", currentPlayer=" << currentPlayer;
    if (pc->owner != currentPlayer) {
        dbg << " Not your piece.";
        m_renderer.drawText(0, 22, dbg.str());
        m_renderer.refreshScreen();
        return;
    }
    m_renderer.drawText(0, 22, dbg.str());
    m_renderer.refreshScreen();

    // Generate moves
    auto moves = m_moveGen.generateMoves(reg);

    // Print all legal moves for this piece
    int moveLine = 23;
    bool found = false;
    for (const auto& m : moves) {
        if (m.fromX == x && m.fromY == y) {
            found = true;
            std::stringstream msg;
            msg << "Legal: (" << (m.fromX+1) << "," << (m.fromY+1) << ") -> (" << (m.toX+1) << "," << (m.toY+1) << ")";
            m_renderer.drawText(0, moveLine++, msg.str());
        }
    }
    if (!found) {
        m_renderer.drawText(0, moveLine++, "No legal moves for this piece.");
    }
    m_renderer.drawText(0, moveLine++, "Enter next x y : ");
    m_renderer.refreshScreen();

    int nx, ny;
    while (!readCoords(nx, ny)) {
        m_renderer.drawText(0, moveLine++, "Invalid input. Enter next x y : ");
        m_renderer.refreshScreen();
    }

    // Validate move
    for (auto& m : moves) {
        if (m.fromX == x && m.fromY == y &&
            m.toX == nx && m.toY == ny) {

            applyMove(reg, m);

            // Send move to opponent
            if (m_opponentPid != 0) {
                std::string path = "/tmp/terminalShogi_move_" + std::to_string(getpid());
                std::ofstream out(path);
                if (out) {
                    out << m.fromX << " " << m.fromY << " " << m.toX << " " << m.toY << std::endl;
                    out.close();
                }
            }

            // Switch turn
            for (auto e : reg.aliveEntities()) {
                auto t = reg.getComponent<TurnComponent>(e);
                if (t) {
                    t->currentPlayer = 1 - t->currentPlayer;
                    break;
                }
            }

            // Notify opponent
            if (m_opponentPid != 0) {
                kill(m_opponentPid, SIGUSR1);
            }

            return;
        }
    }

    m_renderer.drawText(0, moveLine++, "Illegal move. Try again.");
}
