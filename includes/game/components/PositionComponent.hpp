/**==============================================
 *                 PositionComponent.hpp
 *  idk
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/ecs/Component.hpp"

struct PositionComponent : public ecs::IComponent {
    int x = 0;
    int y = 0;
};
