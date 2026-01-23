/**==============================================
 *                 ShogiFactory.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/shogi/ShogiFactory.hpp"
#include "../../../includes/game/shogi/ShogiMoveGenerator.hpp"
#include "../../../includes/game/shogi/ShogiRules.hpp"
#include "../../../includes/game/shogi/ShogiPieces.hpp"
#include "../../../includes/game/systems/RenderSystem.hpp"
#include "../../../includes/game/systems/MovementSystem.hpp"
#include "../../../includes/game/systems/RulesSystem.hpp"
#include "../../../includes/game/systems/CleanupSystem.hpp"

void ShogiFactory::createEntities(ecs::Registry& registry) {
    shogi::createPieces(registry);
}

std::vector<std::unique_ptr<ecs::System>> ShogiFactory::createSystems() {
    std::vector<std::unique_ptr<ecs::System>> systems;

    // Ces systèmes nécessitent des dépendances (renderer, input) injectées dans main
    // Ici on ne met que ceux qui n'ont pas de dépendances externes
    auto rules = new ShogiRules(); // tu peux gérer la durée de vie autrement
    systems.push_back(std::make_unique<RulesSystem>(*rules));
    systems.push_back(std::make_unique<CleanupSystem>());

    return systems;
}

std::unique_ptr<core::MoveGeneratorStrategy> ShogiFactory::createMoveGenerator() {
    return std::make_unique<ShogiMoveGenerator>();
}
