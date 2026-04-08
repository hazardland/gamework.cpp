#include "game/cooldown.h"

Cooldown::Cooldown(int cooldownMs) {
    cooldownTime = std::chrono::milliseconds(cooldownMs);
    lastActionTime = std::chrono::steady_clock::now() - cooldownTime;
}

bool Cooldown::isReady() {
    auto now = std::chrono::steady_clock::now();
    return (now - lastActionTime) > cooldownTime;
}

void Cooldown::reset() {
    lastActionTime = std::chrono::steady_clock::now();
}


