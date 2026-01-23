/**==============================================
 *                 RenderSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/systems/RenderSystem.hpp"
#include "../../../includes/game/components/PositionComponent.hpp"
#include "../../../includes/game/components/PieceComponent.hpp"

RenderSystem::RenderSystem(ui::NcursesRenderer& renderer, const std::vector<char>* captured0, const std::vector<char>* captured1)
    : m_renderer(renderer), m_captured0(captured0), m_captured1(captured1) {}

void RenderSystem::update(ecs::Registry& registry, float) {
    m_renderer.clear();

    const int W = 9;
    const int H = 9;

    // Draw captured pieces for player 0 (left)
    if (m_captured0) {
        std::string cap = "";
        for (char c : *m_captured0) cap += c;
        m_renderer.drawText(0, 0, cap);
    }

    // Draw top border
    m_renderer.drawText(5, 0, "-------------------------------------");

    int rowY = 1;

    for (int y = 0; y < H; ++y) {
        std::string line = "|";

        for (int x = 0; x < W; ++x) {
            char symbol = ' ';

            // find piece at (x,y)
            for (auto e : registry.aliveEntities()) {
                auto pos = registry.getComponent<PositionComponent>(e);
                auto pc  = registry.getComponent<PieceComponent>(e);
                if (!pos || !pc) continue;

                if (pos->x == x && pos->y == y) {
                    symbol = pc->symbol;
                    break;
                }
            }

            line += " ";
            line += symbol;
            line += " |";
        }

        m_renderer.drawText(5, rowY++, line);
        m_renderer.drawText(5, rowY++, "-------------------------------------");
    }

    // Draw captured pieces for player 1 (right)
    if (m_captured1) {
        std::string cap = "";
        for (char c : *m_captured1) cap += c;
        m_renderer.drawText(55, 0, cap);
    }

    m_renderer.refreshScreen();
}

