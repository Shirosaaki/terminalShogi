/**==============================================
 *                 NcursesRenderer.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/ui/NcursesRenderer.hpp"
#include <ncurses.h>

namespace ui {

NcursesRenderer::NcursesRenderer() {
    getmaxyx(stdscr, m_height, m_width);
}

NcursesRenderer::~NcursesRenderer() {
    // rien ici, endwin() est géré par InputSystem
}

void NcursesRenderer::clear() {
    ::clear();
}

void NcursesRenderer::drawCell(int x, int y, char c) {
    mvaddch(y, x, c);
}

void NcursesRenderer::drawText(int x, int y, const std::string& text) {
    mvaddstr(y, x, text.c_str());
}

void NcursesRenderer::refreshScreen() {
    ::refresh();
}

} // namespace ui
