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

int main(int argc, char** argv) {
    // -------------------------------
    // 1. Gestion du PID joueur
    // -------------------------------
    pid_t myPid = getpid();
    std::optional<pid_t> opponentPid;

    if (argc == 1) {
        std::cout << "=== terminalShogi ===\n";
        std::cout << "Mode 2 joueurs : en attente du second joueur...\n";
        std::cout << "PID du joueur 1 : " << myPid << "\n";
        std::cout << "Lancez : ./terminalShogi " << myPid << "\n";
        std::cout << "Appuyez sur Entrée pour continuer en solo.\n";
        std::cin.get();
    } else if (argc == 2) {
        opponentPid = std::stoi(argv[1]);
        std::cout << "=== terminalShogi ===\n";
        std::cout << "Connexion au joueur 1 (PID " << *opponentPid << ")\n";
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

