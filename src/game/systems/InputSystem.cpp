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
    // Use BoardCellComponent as single source of truth for what piece is on a cell
    for (auto e : reg.aliveEntities()) {
        auto bc = reg.getComponent<BoardCellComponent>(e);
        if (!bc) continue;
        if (bc->x == x && bc->y == y) return bc->piece;
    }
    return ecs::INVALID_ENTITY;
}

void InputSystem::applyMove(ecs::Registry& reg, const core::Move& m, bool recordCapture) {
    auto piece = pieceAt(reg, m.fromX, m.fromY);
    if (piece == ecs::INVALID_ENTITY) return;


    // remove captured piece and add to hand
    auto target = pieceAt(reg, m.toX, m.toY);
    if (target != ecs::INVALID_ENTITY) {
        auto pc = reg.getComponent<PieceComponent>(target);
        if (pc) {
            if (recordCapture) {
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
        }

        // Clear any board cell references pointing to the target before destroying it
        for (auto e : reg.aliveEntities()) {
            auto bc = reg.getComponent<BoardCellComponent>(e);
            if (!bc) continue;
            if (bc->piece == target) bc->piece = ecs::INVALID_ENTITY;
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

    // Drop mode: user can type 'd' or 'drop' to drop a captured piece
    if (line == "d" || line == "drop") {
        auto& caps = m_captured[m_localPlayer];
        if (caps.empty()) {
            m_renderer.drawText(0, 21, "No captured pieces to drop.");
            m_renderer.refreshScreen();
            return;
        }

        int sel = 0;
        bool cancelled = false;
        bool chosen = false;
        while (!chosen && !cancelled) {
            // render selection UI
            m_renderer.drawText(0, 21, "Select captured piece (←/→ then Enter, q to cancel)");
            std::string list;
            for (size_t i = 0; i < caps.size(); ++i) {
                if ((int)i == sel) {
                    list += "[";
                    list += caps[i];
                    list += "] ";
                } else {
                    list += " ";
                    list += caps[i];
                    list += "  ";
                }
            }
            m_renderer.drawText(0, 22, list);
            m_renderer.refreshScreen();

            // wait for key event
            bool got = false;
            while (!got) {
                m_input.pollEvents();
                if (m_input.hasEvent()) {
                    auto ev = m_input.nextEvent();
                    if (auto kev = std::get_if<ui::KeyEvent>(&ev)) {
                        int k = kev->key;
                        if (k == KEY_LEFT) { sel = (sel - 1 + (int)caps.size()) % (int)caps.size(); got = true; }
                        else if (k == KEY_RIGHT) { sel = (sel + 1) % (int)caps.size(); got = true; }
                        else if (k == 10 || k == KEY_ENTER || k == 13) { chosen = true; got = true; }
                        else if (k == 'q') { cancelled = true; got = true; }
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        if (cancelled) {
            m_renderer.drawText(0, 21, "Drop cancelled.");
            m_renderer.refreshScreen();
            return;
        }

        // chosen piece symbol
        char chosenSymbol = caps[sel];
        // remove from captured
        caps.erase(caps.begin() + sel);

        // ask destination
        m_renderer.drawText(0, 21, "Drop to: x y");
        m_renderer.refreshScreen();
        std::string dest = m_input.readLineBlocking();
        std::stringstream dss(dest);
        int tx, ty;
        if (!(dss >> tx >> ty)) {
            m_renderer.drawText(0, 22, "Invalid coords.");
            m_renderer.refreshScreen();
            return;
        }
        tx--; ty--;

        // check target empty
        if (pieceAt(reg, tx, ty) != ecs::INVALID_ENTITY) {
            m_renderer.drawText(0, 22, "Target not empty.");
            m_renderer.refreshScreen();
            return;
        }

        // create new piece entity
        auto e = reg.createEntity();
        auto& ppos = reg.addComponent<PositionComponent>(e);
        ppos.x = tx; ppos.y = ty;
        auto& ppc = reg.addComponent<PieceComponent>(e);
        // Map symbol to canonical piece name so move generator recognizes it
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
        ppc.name = symToName(chosenSymbol);
        ppc.symbol = chosenSymbol;
        ppc.owner = m_localPlayer;

        // link to board cell
        for (auto cell : reg.aliveEntities()) {
            auto bc = reg.getComponent<BoardCellComponent>(cell);
            if (bc && bc->x == tx && bc->y == ty) {
                bc->piece = e;
                break;
            }
        }

        // Switch turn
        for (auto e2 : reg.aliveEntities()) {
            auto t = reg.getComponent<TurnComponent>(e2);
            if (t) { t->currentPlayer = 1 - t->currentPlayer; break; }
        }

        // Send drop to opponent
        if (m_opponentPid != 0) {
            std::string path = "/tmp/terminalShogi_move_" + std::to_string(getpid());
            std::ofstream out(path);
            out << "D " << chosenSymbol << " " << tx << " " << ty << "\n";
            out.close();
            kill(m_opponentPid, SIGUSR1);
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
    bool moved = false;
    for (const auto& m : pieceMoves) {
        if (m.toX == tx && m.toY == ty) {
            applyMove(reg, m);

            // Switch turn
            for (auto e : reg.aliveEntities()) {
                auto t = reg.getComponent<TurnComponent>(e);
                if (t) { t->currentPlayer = 1 - t->currentPlayer; break; }
            }

            // Send move
            if (m_opponentPid != 0) {
                std::string path = "/tmp/terminalShogi_move_" + std::to_string(getpid());
                std::ofstream out(path);
                out << "M " << m.fromX << " " << m.fromY << " " << m.toX << " " << m.toY << "\n";
                out.close();
                kill(m_opponentPid, SIGUSR1);
            }

            moved = true;
            break;
        }
    }

    if (!moved) {
        m_renderer.drawText(0, 22, "Invalid move.");
        m_renderer.refreshScreen();
    }
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
            std::string type;
            if (in >> type) {
                if (type == "M") {
                    core::Move m;
                    if (in >> m.fromX >> m.fromY >> m.toX >> m.toY) {
                            applyMove(reg, m, false);
                        // Switch turn to local
                        for (auto e : reg.aliveEntities()) {
                            auto t = reg.getComponent<TurnComponent>(e);
                            if (t) {
                                t->currentPlayer = m_localPlayer;
                                break;
                            }
                        }
                    }
                } else if (type == "D") {
                    char sym;
                    int tx, ty;
                    if (in >> sym >> tx >> ty) {
                        // create dropped piece for remote player
                        auto e = reg.createEntity();
                        auto& ppos = reg.addComponent<PositionComponent>(e);
                        ppos.x = tx; ppos.y = ty;
                        auto& ppc = reg.addComponent<PieceComponent>(e);
                        // Map symbol to canonical piece name so move generator recognizes it
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
                        ppc.name = symToName(sym);
                        ppc.symbol = sym;
                        ppc.owner = 1 - m_localPlayer; // sender is the other player

                        // link to board cell
                        for (auto cell : reg.aliveEntities()) {
                            auto bc = reg.getComponent<BoardCellComponent>(cell);
                            if (bc && bc->x == tx && bc->y == ty) {
                                bc->piece = e;
                                break;
                            }
                        }

                        // Switch turn to local
                        for (auto e2 : reg.aliveEntities()) {
                            auto t = reg.getComponent<TurnComponent>(e2);
                            if (t) { t->currentPlayer = m_localPlayer; break; }
                        }
                    }
                }
            }
            in.close();
            std::remove(path.c_str());
        }
    }
}
