/**==============================================
 *                 InputSystem.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/ui/InputSystem.hpp"
#include <ncurses.h>
#include <string>

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

std::string InputSystem::readLineBlocking() {
    char buffer[256] = {0};

    // Place input below the board (board is 9x9, each row is 2 lines, plus border)
    int input_y = 22; // 9*2+4 for border and spacing
    int input_x = 0;

    // Clear the input line
    move(input_y, input_x);
    clrtoeol();

    // Temporarily switch to blocking mode and enable echo for line input
    nodelay(stdscr, FALSE);
    echo();
    mvgetnstr(input_y, input_x, buffer, 255);
    noecho();
    nodelay(stdscr, TRUE);

    return std::string(buffer);
}

} // namespace ui
