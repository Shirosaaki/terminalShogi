/**==============================================
 *                 ShogiFactory.hpp
 *  facto
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "../../core/patterns/GameFactory.hpp"

class ShogiFactory : public core::GameFactory {
public:
    void createEntities(ecs::Registry& registry) override;
    std::vector<std::unique_ptr<ecs::System>> createSystems() override;
    std::unique_ptr<core::MoveGeneratorStrategy> createMoveGenerator() override;
};
