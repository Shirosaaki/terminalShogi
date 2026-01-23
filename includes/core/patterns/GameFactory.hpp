/**==============================================
 *                 GameFactory.hpp
 *  the game factory
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include <memory>
#include <vector>
#include "../ecs/System.hpp"
#include "MoveGeneratorStrategy.hpp"

namespace core {

class GameFactory {
public:
    virtual ~GameFactory() = default;

    virtual void createEntities(ecs::Registry& registry) = 0;

    virtual std::vector<std::unique_ptr<ecs::System>>
    createSystems() = 0;

    virtual std::unique_ptr<MoveGeneratorStrategy>
    createMoveGenerator() = 0;
};

} // namespace core
