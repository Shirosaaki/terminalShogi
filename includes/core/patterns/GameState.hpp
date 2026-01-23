/**==============================================
 *                 GameState.hpp
 *  know the current state and modify it
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "core/ecs/Registry.hpp"

namespace core {

class GameState {
public:
    virtual ~GameState() = default;
    virtual void update(ecs::Registry& registry) = 0;
    virtual bool isGameOver() const = 0;
};

} // namespace core
