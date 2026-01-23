/**==============================================
 *                 InputSystem.hpp
 *  to play 
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "UIEvents.hpp"
#include <queue>

namespace ui {

class InputSystem {
public:
    InputSystem();
    ~InputSystem();

    void pollEvents();               // lit ncurses et remplit la queue
    bool hasEvent() const;
    UiEvent nextEvent();

private:
    std::queue<UiEvent> m_events;
};

} // namespace ui
