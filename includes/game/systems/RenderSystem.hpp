/**==============================================
 *                 RenderSystem.hpp
 *  render is better
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "../../core/ecs/System.hpp"
#include "../../ui/NcursesRenderer.hpp"

class RenderSystem : public ecs::System {
public:
    RenderSystem(ui::NcursesRenderer& renderer);

    void update(ecs::Registry& registry, float dt) override;

private:
    ui::NcursesRenderer& m_renderer;
};
