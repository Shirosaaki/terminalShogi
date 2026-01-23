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
    RenderSystem(ui::NcursesRenderer& renderer, const std::vector<char>* captured0 = nullptr, const std::vector<char>* captured1 = nullptr);

    void update(ecs::Registry& registry, float dt) override;

    void setCaptured(const std::vector<char>* c0, const std::vector<char>* c1) { m_captured0 = c0; m_captured1 = c1; }

private:
    ui::NcursesRenderer& m_renderer;
    const std::vector<char>* m_captured0;
    const std::vector<char>* m_captured1;
};
