#ifndef COOLDOWN_H
#define COOLDOWN_H

#include <chrono>

class Cooldown {
public:
    Cooldown(int cooldownMs);  // Regular constructor

    bool isReady();
    bool isActive();
    void reset();

private:
    std::chrono::milliseconds cooldownTime;
    std::chrono::steady_clock::time_point lastActionTime;
};

#endif


