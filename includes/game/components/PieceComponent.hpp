/**==============================================
 *                 PieceComponent.hpp
 *  ink
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/ecs/Component.hpp"
#include <string>

struct PieceComponent : public ecs::IComponent {
    std::string name;
    char symbol = '?';
    int owner = 0; // joueur 0 ou 1
};
