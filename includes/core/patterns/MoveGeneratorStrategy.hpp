/**==============================================
 *                 MoveGeneratorStrategy.hpp
 *  idk
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include <vector>
#include "core/ecs/Registry.hpp"

namespace core {

struct Move {
    int fromX, fromY;
    int toX, toY;
    bool promotion = false;
};

class MoveGeneratorStrategy {
public:
    virtual ~MoveGeneratorStrategy() = default;
    virtual std::vector<Move> generateMoves(ecs::Registry& registry) = 0;
};

} // namespace core
