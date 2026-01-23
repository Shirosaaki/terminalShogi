/**==============================================
 *                 InputSystem.hpp
 *  can you press ?
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "../../core/ecs/System.hpp"
#include "../../core/patterns/MoveGeneratorStrategy.hpp"
#include "../../ui/NcursesRenderer.hpp"

namespace ui { class InputSystem; }

class InputSystem : public ecs::System {
public:
    InputSystem(core::MoveGeneratorStrategy& moveGen,
                ui::NcursesRenderer& renderer,
                ui::InputSystem& input,
                int localPlayer,
                pid_t opponentPid);

    void update(ecs::Registry& registry, float dt) override;

private:
    core::MoveGeneratorStrategy& m_moveGen;
    ui::NcursesRenderer& m_renderer;
    ui::InputSystem& m_input;
    int m_localPlayer;
    pid_t m_opponentPid;

    ecs::Entity pieceAt(ecs::Registry& reg, int x, int y);
    void applyMove(ecs::Registry& reg, const core::Move& m);
    void handleMyTurn(ecs::Registry& reg);
    void handleOpponentTurn(ecs::Registry& reg);
};
