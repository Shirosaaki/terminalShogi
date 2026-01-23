/**==============================================
 *                 SignalHandler.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/core/utils/SignalHandler.hpp"
#include <csignal>

#include <utility>

#include <atomic>

namespace {
std::atomic<bool> g_userSignalFlag{false};
std::atomic<pid_t> g_lastUserSender{0};
}

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

static void handleSigUsr1(int, siginfo_t* info, void*) {
    if (info) {
        g_lastUserSender.store(static_cast<pid_t>(info->si_pid));
    }
    g_userSignalFlag.store(true);
    if (core::SignalHandler::s_userCallback)
        core::SignalHandler::s_userCallback();
}

void SignalHandler::init() {
    std::signal(SIGINT, handleSigInt);

    struct sigaction sa{};
    sa.sa_sigaction = handleSigUsr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, nullptr);
}

void SignalHandler::setUserCallback(std::function<void()> cb) {
    s_userCallback = cb;
}

bool SignalHandler::consumeUserSignal() {
    return g_userSignalFlag.exchange(false);
}

pid_t SignalHandler::lastUserSender() {
    return g_lastUserSender.load();
}

} // namespace core
