/**==============================================
 *                 GameStatusComponent.hpp
 *  But, would you loose ?
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/ecs/Component.hpp"

struct GameStatusComponent : public ecs::IComponent {
    bool gameOver = false;
    int winner = -1; // -1 = aucun
};
