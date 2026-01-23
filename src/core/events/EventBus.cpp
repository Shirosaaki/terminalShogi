/**==============================================
 *                 EventBus.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/core/events/EventBus.hpp"
#include <algorithm>

namespace core {

void EventBus::subscribe(Observer* obs) {
    m_observers.push_back(obs);
}

void EventBus::unsubscribe(Observer* obs) {
    m_observers.erase(
        std::remove(m_observers.begin(), m_observers.end(), obs),
        m_observers.end()
    );
}

void EventBus::emit(const Event& e) {
    for (auto* obs : m_observers) {
        obs->onEvent(e);
    }
}

} // namespace core

