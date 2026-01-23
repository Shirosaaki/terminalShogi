/**==============================================
 *                 UIEvents.hpp
 *  you need to get what happens to play
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include <variant>

namespace ui {

struct KeyEvent {
    int key;
};

struct ResizeEvent {
    int width;
    int height;
};

struct QuitEvent {};

using UiEvent = std::variant<KeyEvent, ResizeEvent, QuitEvent>;

} // namespace ui
