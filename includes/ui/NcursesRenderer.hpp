/**==============================================
 *                 NcursesRenderer.hpp
 *  for a better rendering
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include <vector>
#include <string>

namespace ui {

class NcursesRenderer {
public:
    NcursesRenderer();
    ~NcursesRenderer();

    void clear();
    void drawCell(int x, int y, char c);
    void drawText(int x, int y, const std::string& text);
    void refreshScreen();

private:
    int m_width;
    int m_height;
};

} // namespace ui
