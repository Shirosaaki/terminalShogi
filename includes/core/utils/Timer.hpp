/**==============================================
 *                 Timer.hpp
 *  the timer
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include <chrono>

namespace core {

class Timer {
public:
    Timer();

    void reset();
    float elapsed() const;

private:
    std::chrono::steady_clock::time_point m_start;
};

} // namespace core
