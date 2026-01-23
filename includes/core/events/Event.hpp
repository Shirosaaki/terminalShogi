/**==============================================
 *                 Event.hpp
 *  event handling
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include <string>

namespace core {

struct Event {
    std::string type;
    // Tu peux ajouter un std::variant payload plus tard
};

} // namespace core
