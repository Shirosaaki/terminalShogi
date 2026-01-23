/**==============================================
 *                 BoardCellComponent.hpp
 *  the board cell
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/ecs/Component.hpp"
#include "../../core/ecs/Entity.hpp"

struct BoardCellComponent : public ecs::IComponent {
    int x = 0;
    int y = 0;
    ecs::Entity piece = ecs::INVALID_ENTITY;
};
