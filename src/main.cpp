/**==============================================
 *                 main.cpp
 *  main of the terminalShogi project
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include <iostream>

int main() {
    Registry registry;

    std::unique_ptr<GameFactory> factory = std::make_unique<ShogiFactory>();
    factory->createEntities(registry);
    auto systems = factory->createSystems();

    while (true) {
        for (auto& sys : systems)
            sys->update(registry, 0.016f);
    }
}
