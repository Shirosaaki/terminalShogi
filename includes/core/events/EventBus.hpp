/**==============================================
 *                 EventBus.hpp
 *  file where the event will be receveide and send
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include "Event.hpp"
#include "Observer.hpp"
#include <vector>

namespace core {

class EventBus {
public:
    void subscribe(Observer* obs);
    void unsubscribe(Observer* obs);
    void emit(const Event& e);

private:
    std::vector<Observer*> m_observers;
};

} // namespace core
