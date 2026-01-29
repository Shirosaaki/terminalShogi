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

    // Get captured pieces for a player (0 or 1)
    const std::vector<char>& getCaptured(int player) const { return m_captured[player]; }

private:
    core::MoveGeneratorStrategy& m_moveGen;
    ui::NcursesRenderer& m_renderer;
    ui::InputSystem& m_input;
    int m_localPlayer;
    pid_t m_opponentPid;

    // Store captured pieces for each player (0: left, 1: right)
    std::vector<char> m_captured[2];

    ecs::Entity pieceAt(ecs::Registry& reg, int x, int y);
    // recordCapture: when true, the captured piece is added to the local captured list
    // promptPromotion: when true, asks the local player whether to promote when applicable
    void applyMove(ecs::Registry& reg, const core::Move& m, bool recordCapture = true, bool promptPromotion = false);
    void handleMyTurn(ecs::Registry& reg);
    void handleOpponentTurn(ecs::Registry& reg);
};
