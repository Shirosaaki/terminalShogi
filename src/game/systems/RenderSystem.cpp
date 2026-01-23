/**==============================================
 *                 RenderSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/systems/RenderSystem.hpp"
#include "../../../includes/game/components/PositionComponent.hpp"
#include "../../../includes/game/components/PieceComponent.hpp"

RenderSystem::RenderSystem(ui::NcursesRenderer& renderer)
    : m_renderer(renderer) {}

void RenderSystem::update(ecs::Registry& registry, float) {
    m_renderer.clear();

    const int W = 9;
    const int H = 9;

    // Draw top border
    m_renderer.drawText(0, 0, "-------------------------------------");

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

        m_renderer.drawText(0, rowY++, line);
        m_renderer.drawText(0, rowY++, "-------------------------------------");
    }

    m_renderer.refreshScreen();
}

