/**==============================================
 *                 ShogiPieces.hpp
 *  My trowns is here
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/ecs/Registry.hpp"

namespace shogi {

void createBoard(ecs::Registry& registry);
void createPieces(ecs::Registry& registry);

}
