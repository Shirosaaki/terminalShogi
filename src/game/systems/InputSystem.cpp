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
#include <fcntl.h>
#include <unistd.h>
#include "../../../includes/core/utils/SignalHandler.hpp"

InputSystem::InputSystem(core::MoveGeneratorStrategy& moveGen,
                         ui::NcursesRenderer& renderer,
                         ui::InputSystem& input,
                         int localPlayer,
                         pid_t opponentPid)
    : m_moveGen(moveGen), m_renderer(renderer), m_input(input), m_localPlayer(localPlayer), m_opponentPid(opponentPid) {}

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


    // remove captured piece and add to hand
    auto target = pieceAt(reg, m.toX, m.toY);
    if (target != ecs::INVALID_ENTITY) {
        auto pc = reg.getComponent<PieceComponent>(target);
        if (pc) {
            char capturedSymbol = pc->symbol;
            // Determine current player from TurnComponent
            int currentPlayer = 0;
            for (auto e : reg.aliveEntities()) {
                auto t = reg.getComponent<TurnComponent>(e);
                if (t) { currentPlayer = t->currentPlayer; break; }
            }
            if (currentPlayer == 0)
                capturedSymbol = std::toupper(capturedSymbol);
            else
                capturedSymbol = std::tolower(capturedSymbol);
            m_captured[currentPlayer].push_back(capturedSymbol);
        }
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

    // Poll UI events
    m_input.pollEvents();

    // Get current player
    int currentPlayer = 0;
    for (auto e : reg.aliveEntities()) {
        auto t = reg.getComponent<TurnComponent>(e);
        if (t) {
            currentPlayer = t->currentPlayer;
            break;
        }
    }

    if (currentPlayer == m_localPlayer) {
        handleMyTurn(reg);
    } else {
        handleOpponentTurn(reg);
    }
}

void InputSystem::handleMyTurn(ecs::Registry& reg) {
    m_renderer.drawText(0, 20, "Select piece: x y or 'exit'");
    m_renderer.refreshScreen();

    std::string line = m_input.readLineBlocking();
    if (line == "exit") {
        for (auto e : reg.aliveEntities()) {
            auto gs = reg.getComponent<GameStatusComponent>(e);
            if (gs) {
                gs->gameOver = true;
                break;
            }
        }

        // notify opponent to exit as well
        if (m_opponentPid != 0) {
            std::string path = "/tmp/terminalShogi_exit_" + std::to_string(getpid());
            int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (fd >= 0) {
                std::string payload = "exit\n";
                ::write(fd, payload.c_str(), payload.size());
                ::fsync(fd);
                ::close(fd);
            }

            // wake opponent (SIGUSR1) so they check files, then send SIGINT to request clean shutdown
            kill(m_opponentPid, SIGUSR1);
            kill(m_opponentPid, SIGINT);
            // request local shutdown as well
            kill(getpid(), SIGINT);
        }

        return;
    }

    std::stringstream ss(line);
    int fx, fy;
    if (!(ss >> fx >> fy)) {
        m_renderer.drawText(0, 21, "Invalid input.");
        m_renderer.refreshScreen();
        return;
    }
    fx--; fy--;

    ecs::Entity piece = pieceAt(reg, fx, fy);
    if (piece == ecs::INVALID_ENTITY) {
        m_renderer.drawText(0, 21, "No piece there.");
        m_renderer.refreshScreen();
        return;
    }

    auto pc = reg.getComponent<PieceComponent>(piece);
    if (!pc || pc->owner != m_localPlayer) {
        m_renderer.drawText(0, 21, "Not your piece.");
        m_renderer.refreshScreen();
        return;
    }

    // Generate moves
    auto allMoves = m_moveGen.generateMoves(reg);
    std::vector<core::Move> pieceMoves;
    for (const auto& m : allMoves) {
        if (m.fromX == fx && m.fromY == fy) {
            pieceMoves.push_back(m);
            // Draw '.' at to position: col = 5 + 2 + x*4, row = 1 + y*2 (to match RenderSystem offset)
            int row = 1 + m.toY * 2;
            int col = 5 + 2 + m.toX * 4;
            m_renderer.drawCell(col, row, '.');
        }
    }
    m_renderer.refreshScreen();

    // Prompt for destination
    m_renderer.drawText(0, 21, "Move to: x y");
    m_renderer.refreshScreen();

    line = m_input.readLineBlocking();
    ss.str(line);
    ss.clear();
    int tx, ty;
    if (!(ss >> tx >> ty)) {
        m_renderer.drawText(0, 22, "Invalid input.");
        m_renderer.refreshScreen();
        return;
    }
    tx--; ty--;

    // Check move
    for (const auto& m : pieceMoves) {
        if (m.toX == tx && m.toY == ty) {
            applyMove(reg, m);

            // Switch turn
            for (auto e : reg.aliveEntities()) {
                auto t = reg.getComponent<TurnComponent>(e);
                if (t) {
                    t->currentPlayer = 1 - t->currentPlayer;
                    break;
                }
            }

            // Send move
            if (m_opponentPid != 0) {
                std::string path = "/tmp/terminalShogi_move_" + std::to_string(getpid());
                std::ofstream out(path);
                out << m.fromX << " " << m.fromY << " " << m.toX << " " << m.toY << "\n";
                out.close();
                kill(m_opponentPid, SIGUSR1);
            }

            return;
        }
    }

    m_renderer.drawText(0, 22, "Invalid move.");
    m_renderer.refreshScreen();
}

void InputSystem::handleOpponentTurn(ecs::Registry& reg) {
    static bool firstWait = true;
    if (firstWait && m_localPlayer == 0) {
        firstWait = false;
        return; // Player 1 starts
    }

    m_renderer.drawText(0, 20, "Waiting for opponent...");
    m_renderer.refreshScreen();

    // Wait for signal
    while (!core::SignalHandler::consumeUserSignal()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Receive move
    pid_t sender = core::SignalHandler::lastUserSender();
    if (sender != 0) {
        // First check for an exit notification
        std::string exitPath = "/tmp/terminalShogi_exit_" + std::to_string(sender);
        if (access(exitPath.c_str(), F_OK) == 0) {
            // Mark game over locally
            for (auto e : reg.aliveEntities()) {
                auto gs = reg.getComponent<GameStatusComponent>(e);
                if (gs) { gs->gameOver = true; break; }
            }
            // remove exit file
            std::remove(exitPath.c_str());
            // request local shutdown via SIGINT to trigger main running=false
            kill(getpid(), SIGINT);
            return;
        }

        std::string path = "/tmp/terminalShogi_move_" + std::to_string(sender);
        std::ifstream in(path);
        if (in) {
            core::Move m;
            if (in >> m.fromX >> m.fromY >> m.toX >> m.toY) {
                applyMove(reg, m);

                // Switch turn to local
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
}
