/**==============================================
 *                 main.cpp
 *  main of the terminalShogi project
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../includes/my.h"
#include <iostream>
#include <string>
#include <unistd.h>     // getpid()
#include <optional>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {
    pid_t myPid = getpid();
    std::optional<pid_t> opponentPid;

    bool secondPlayerConnected = false;

    // Installer le callback SIGUSR1 AVANT d’attendre
    core::SignalHandler::init();
    core::SignalHandler::setUserCallback([&](){
        secondPlayerConnected = true;
    });

    if (argc == 1) {
        std::cout << "=== terminalShogi ===\n";
        std::cout << "Waiting for second player...\n";
        std::cout << "PID: " << myPid << "\n";
        std::cout << "Launch: ./terminalShogi " << myPid << "\n";

        while (!secondPlayerConnected) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Second player connected!\n";
    }
    else if (argc == 2) {
        opponentPid = std::stoi(argv[1]);
        std::cout << "Connecting to player 1 (PID " << *opponentPid << ")\n";

        // On notifie le joueur 1
        kill(*opponentPid, SIGUSR1);
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


    // Injection manuelle des systèmes dépendants de l’UI
    systems.push_back(std::make_unique<InputSystem>(input));
    systems.push_back(std::make_unique<RenderSystem>(renderer));
    systems.push_back(std::make_unique<CleanupSystem>());

    // -------------------------------
    // 5. Boucle principale
    // -------------------------------
    while (running) {
        for (auto& sys : systems) {
            sys->update(registry, 0.016f);
        }
    }

    return 0;
}

