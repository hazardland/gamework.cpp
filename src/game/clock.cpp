#include "game/clock.h"

void Clock::tick() {
    Uint64 ticks = SDL_GetTicks();
    delta = ticks - previousTicks;
    previousTicks = ticks;
    fps = (delta > 0) ? 1000.0f / delta : 0.0f;
}


