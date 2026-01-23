/**==============================================
 *                 Observer.hpp
 *  the observer design pattern
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "Event.hpp"

namespace core {

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onEvent(const Event& e) = 0;
};

} // namespace core
