/**==============================================
 *                 main.cpp
 *  main of the terminalShogi project
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../includes/my.h"
#include <iostream>
#include <string>
#include <ncurses.h>
#include <unistd.h>     // getpid()
#include <optional>
#include <thread>
#include <chrono>
#include "../includes/game/components/GameStatusComponent.hpp"

int main(int argc, char** argv) {
    pid_t myPid = getpid();
    std::optional<pid_t> opponentPid;
    pid_t opponentPidValue = 0;
    int localPlayer = 0; // 0 = player 1, 1 = player 2

    bool secondPlayerConnected = false;

    // Installer le callback SIGUSR1 AVANT d’attendre
    core::SignalHandler::init();
    core::SignalHandler::setUserCallback([&](){
        secondPlayerConnected = true;
    });

    if (argc == 1) {
        std::cout << "=== terminalShogi ===\n";
        std::cout << "You are Player 1 (bottom, plays first)." << std::endl;
        std::cout << "Waiting for second player...\n";
        std::cout << "PID: " << myPid << "\n";
        std::cout << "Launch: ./terminalShogi " << myPid << "\n";
        localPlayer = 0;
        opponentPidValue = 0; // Will be set by player 2

        while (!secondPlayerConnected) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        // retrieve the opponent PID from the signal info
        opponentPidValue = core::SignalHandler::lastUserSender();
        std::cout << "Second player connected! PID=" << opponentPidValue << "\n";
    }
    else if (argc == 2) {
        opponentPid = std::stoi(argv[1]);
        opponentPidValue = *opponentPid;
        std::cout << "You are Player 2 (top, plays second)." << std::endl;
        std::cout << "Connecting to player 1 (PID " << *opponentPid << ")\n";

        // On notifie le joueur 1
        kill(*opponentPid, SIGUSR1);
        localPlayer = 1;
    } else {
        std::cerr << "Usage: ./terminalShogi [pid]\n";
        return 1;
    }

    // -------------------------------
    // 2. Initialisation du SignalHandler
    // -------------------------------
    bool running = true;
    core::SignalHandler::init();
    core::SignalHandler::setInterruptCallback([&]() {
        running = false;
    });

    // -------------------------------
    // 3. Initialisation UI
    // -------------------------------
    ui::InputSystem input;
    ui::NcursesRenderer renderer;

    // -------------------------------
    // 4. ECS + Factory Shogi
    // -------------------------------
    ecs::Registry registry;

    ShogiFactory factory;
    factory.createEntities(registry);

    auto moveGen = factory.createMoveGenerator();
    auto systems = factory.createSystems();

    // Create InputSystem first so we can pass captured pieces to RenderSystem
    // Add movement system that uses the same move generator
    systems.push_back(std::make_unique<MovementSystem>(*moveGen));

    auto inputSystem = std::make_unique<InputSystem>(*moveGen, renderer, input, localPlayer, opponentPidValue);
    // RenderSystem needs to know about captured pieces
    auto renderSystem = std::make_unique<RenderSystem>(renderer, &inputSystem->getCaptured(0), &inputSystem->getCaptured(1));

    systems.push_back(std::move(renderSystem));
    systems.push_back(std::move(inputSystem));

    // -------------------------------
    // 5. Boucle principale
    // -------------------------------
    while (running) {
        for (auto& sys : systems) {
            sys->update(registry, 0.016f);
        }

        // Check game status and notify opponent if needed
        for (auto e : registry.aliveEntities()) {
            auto gs = registry.getComponent<GameStatusComponent>(e);
            if (gs && gs->gameOver) {
                int winner = gs->winner;
                // Ensure ncurses is ended so stdout is visible
                endwin();
                std::cout << "Game over. Winner: Player " << (winner + 1) << "\n";

                // notify opponent with winner info if there is an opponent
                if (opponentPidValue != 0) {
                    std::string path = "/tmp/terminalShogi_exit_" + std::to_string(getpid());
                    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
                    if (fd >= 0) {
                        std::string payload = "win " + std::to_string(winner) + "\n";
                        ::write(fd, payload.c_str(), payload.size());
                        ::fsync(fd);
                        ::close(fd);
                    }
                    kill(opponentPidValue, SIGUSR1);
                    kill(opponentPidValue, SIGINT);
                }

                // exit local
                running = false;
                break;
            }
        }
    }

    return 0;
}

