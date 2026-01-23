/**==============================================
 *                 ShogiMoveGenerator.hpp
 *  movement
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/patterns/MoveGeneratorStrategy.hpp"

class ShogiMoveGenerator : public core::MoveGeneratorStrategy {
public:
    std::vector<core::Move> generateMoves(ecs::Registry& registry) override;
};
