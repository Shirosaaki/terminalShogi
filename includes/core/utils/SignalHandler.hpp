/**==============================================
 *                 SignalHandler.hpp
 *  signal handling
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include <functional>

namespace core {

class SignalHandler {
public:
    static void init();
    static void setInterruptCallback(std::function<void()> cb);
    static void setUserCallback(std::function<void()> cb);
    static std::function<void()> s_interruptCallback;
    static std::function<void()> s_userCallback;
private:
};

} // namespace core
