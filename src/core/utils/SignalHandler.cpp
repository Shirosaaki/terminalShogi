/**==============================================
 *                 SignalHandler.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../includes/core/utils/SignalHandler.hpp"
#include <csignal>

namespace core {

std::function<void()> SignalHandler::s_interruptCallback = nullptr;

static void handleSigInt(int) {
    if (core::SignalHandler::s_interruptCallback) {
        core::SignalHandler::s_interruptCallback();
    }
}

void SignalHandler::init() {
    std::signal(SIGINT, handleSigInt);
}

void SignalHandler::setInterruptCallback(std::function<void()> cb) {
    s_interruptCallback = cb;
}

} // namespace core
