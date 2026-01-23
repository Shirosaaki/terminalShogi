/**==============================================
 *                 InputSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/game/systems/InputSystem.hpp"
#include "../../../includes/ui/UIEvents.hpp"

InputSystem::InputSystem(ui::InputSystem& input)
    : m_input(input) {}

void InputSystem::update(ecs::Registry& registry, float) {
    m_input.pollEvents();

    while (m_input.hasEvent()) {
        auto ev = m_input.nextEvent();

        // Ici tu traduis UiEvent → Command
        // Exemple minimal :
        if (std::holds_alternative<ui::QuitEvent>(ev)) {
            // TODO: créer une QuitCommand
        }
    }

    while (!m_commands.empty()) {
        m_commands.front()->execute(registry);
        m_commands.pop();
    }
}
