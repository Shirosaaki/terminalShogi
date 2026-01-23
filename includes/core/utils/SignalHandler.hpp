/**==============================================
 *                 SignalHandler.hpp
 *  signal handling
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once

#include <functional>

#include <atomic>

using pid_t = int;

namespace core {

class SignalHandler {
public:
    static void init();
    static void setInterruptCallback(std::function<void()> cb);
    static void setUserCallback(std::function<void()> cb);
    static std::function<void()> s_interruptCallback;
    static std::function<void()> s_userCallback;
    static bool consumeUserSignal();
    static pid_t lastUserSender();
private:
};

} // namespace core
