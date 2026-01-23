/**==============================================
 *                 RenderSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/game/systems/RenderSystem.hpp"
#include "../../includes/game/components/PositionComponent.hpp"
#include "../../includes/game/components/PieceComponent.hpp"

RenderSystem::RenderSystem(ui::NcursesRenderer& renderer)
    : m_renderer(renderer) {}

void RenderSystem::update(ecs::Registry& registry, float) {
    m_renderer.clear();

    for (auto e : registry.aliveEntities()) {
        auto pos = registry.getComponent<PositionComponent>(e);
        auto piece = registry.getComponent<PieceComponent>(e);

        if (pos && piece) {
            m_renderer.drawCell(pos->x, pos->y, piece->symbol);
        }
    }

    m_renderer.refreshScreen();
}
