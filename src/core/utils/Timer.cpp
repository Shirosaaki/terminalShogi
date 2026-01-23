/**==============================================
 *                 Timer.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/core/utils/Timer.hpp"

namespace core {

Timer::Timer() {
    reset();
}

void Timer::reset() {
    m_start = std::chrono::steady_clock::now();
}

float Timer::elapsed() const {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = now - m_start;
    return diff.count();
}

} // namespace core
