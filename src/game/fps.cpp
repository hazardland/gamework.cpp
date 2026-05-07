// File: game/fps.cpp
#include "game/fps.h"

#include <SDL3/SDL_timer.h>

#include <game/context.h>
#include <game/clock.h>
#include <game/camera.h>
#include <game/screen.h>

// Fps::Fps() {
//     cooldown = new Cooldown(200);
// }

void Fps::update(Context* context) {
        if (cooldown->isReady()) {
            setText(std::to_string(context->clock->fps) + " FPS / DELTA " + std::to_string(context->clock->delta));
            setX(static_cast<float>(context->screen->getWidth() - getWidth() - 5));
            cooldown->reset();
        }

}

bool Fps::isVisible(Context* /* state */) {
    return true;
}


