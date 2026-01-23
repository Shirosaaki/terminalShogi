/**==============================================
 *                 InputSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/ui/InputSystem.hpp"
#include <ncurses.h>

namespace ui {

InputSystem::InputSystem() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // getch() non bloquant
}

InputSystem::~InputSystem() {
    endwin();
}

void InputSystem::pollEvents() {
    int ch = getch();
    if (ch != ERR) {
        m_events.push(KeyEvent{ch});
    }

    // gestion du resize
    int newW, newH;
    getmaxyx(stdscr, newH, newW);
    static int oldW = newW;
    static int oldH = newH;

    if (newW != oldW || newH != oldH) {
        m_events.push(ResizeEvent{newW, newH});
        oldW = newW;
        oldH = newH;
    }

    // touche 'q' = QuitEvent
    if (ch == 'q') {
        m_events.push(QuitEvent{});
    }
}

bool InputSystem::hasEvent() const {
    return !m_events.empty();
}

UiEvent InputSystem::nextEvent() {
    UiEvent e = m_events.front();
    m_events.pop();
    return e;
}

} // namespace ui
