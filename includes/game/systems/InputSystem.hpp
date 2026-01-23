/**==============================================
 *                 InputSystem.hpp
 *  can you press ?
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "../../core/ecs/System.hpp"
#include "../../ui/InputSystem.hpp"
#include "../../core/patterns/Command.hpp"
#include <queue>
#include <memory>

class InputSystem : public ecs::System {
public:
    InputSystem(ui::InputSystem& input);

    void update(ecs::Registry& registry, float dt) override;

private:
    ui::InputSystem& m_input;
    std::queue<std::unique_ptr<core::Command>> m_commands;
};
