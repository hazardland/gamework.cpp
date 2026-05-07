#include "game/clock.h"

void Clock::tick() {
    Uint64 ticks = SDL_GetTicks();
    if (previousTicks == 0) {
        previousTicks = ticks;
        delta = 0;
        fps = 0;
        return;
    }
    delta = ticks - previousTicks;
    previousTicks = ticks;
    fps = (delta > 0) ? 1000.0f / delta : 0.0f;
}


