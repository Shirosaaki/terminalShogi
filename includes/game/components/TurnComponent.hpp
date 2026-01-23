/**==============================================
 *                 TurnComponent.hpp
 *  know where is turn
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/ecs/Component.hpp"

struct TurnComponent : public ecs::IComponent {
    int currentPlayer = 0;
};
