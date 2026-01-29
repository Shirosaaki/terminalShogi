/**==============================================
 *                 ShogiLogic.hpp
 *  game logic utilities
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../../includes/core/patterns/MoveGeneratorStrategy.hpp"
#include "../../../includes/core/ecs/Registry.hpp"
#include "../../../includes/game/components/PieceComponent.hpp"
#include <vector>

namespace shogi {

bool in_promotion_zone(int owner, int y);
bool can_promote(ecs::Registry& reg, ecs::Entity piece, int toY);
bool mandatory_promote(ecs::Registry& reg, ecs::Entity piece, int toY);
// input and renderer may be null; pass ui pointers as void* to avoid heavy headers in this API
void handle_promotion(ecs::Registry& reg, ecs::Entity piece, int toY,
                      void* input, void* renderer);

bool validate_drop(ecs::Registry& reg,
                   char symbol,
                   int owner,
                   int tx, int ty,
                   core::MoveGeneratorStrategy& moveGen,
                   const std::vector<char> hands[2]);

bool is_in_check(ecs::Registry& reg, int player, core::MoveGeneratorStrategy& moveGen);
bool is_checkmate(ecs::Registry& reg, int player, core::MoveGeneratorStrategy& moveGen,
                  const std::vector<char> hands[2]);

} // namespace shogi
