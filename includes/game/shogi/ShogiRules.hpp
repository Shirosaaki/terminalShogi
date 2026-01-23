/**==============================================
 *                 ShogiRules.hpp
 *  rules are law
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../../core/patterns/GameState.hpp"

class ShogiRules : public core::GameState {
public:
    void update(ecs::Registry& registry) override;
    bool isGameOver() const override;

private:
    bool m_over = false;
};
