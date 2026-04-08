#ifndef GAME_CLOCK_H
#define GAME_CLOCK_H

#include <SDL3/SDL_timer.h>

/**
 * @brief Simple clock for fps tracking.
 * It calculates the number of frames per second (fps) and the time delta.
 */
class Clock {
    private:
        Uint64 previousTicks = 0;

    public:
        Uint64 delta = 0;    ///< Time difference in milliseconds between the current and the previous tick.
        Uint64 fps = 0;      ///< The number of frames per second.

        /**
         * @brief Updates delta and fps values based on the current tick.
         */
        void tick();
};

#endif // GAME_CLOCK_H


