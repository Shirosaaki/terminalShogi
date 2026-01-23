/**==============================================
 *                 SignalHandler.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/core/utils/SignalHandler.hpp"
#include <csignal>

namespace core {

std::function<void()> SignalHandler::s_interruptCallback = nullptr;

static void handleSigInt(int) {
    if (core::SignalHandler::s_interruptCallback) {
        core::SignalHandler::s_interruptCallback();
    }
}

void SignalHandler::setInterruptCallback(std::function<void()> cb) {
    s_interruptCallback = cb;
}

std::function<void()> SignalHandler::s_userCallback = nullptr;

static void handleSigUsr1(int) {
    if (core::SignalHandler::s_userCallback)
        core::SignalHandler::s_userCallback();
}

void SignalHandler::init() {
    std::signal(SIGINT, handleSigInt);
    std::signal(SIGUSR1, handleSigUsr1);
}

void SignalHandler::setUserCallback(std::function<void()> cb) {
    s_userCallback = cb;
}

} // namespace core
