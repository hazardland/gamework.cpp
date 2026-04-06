#include "klad1/ladder.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/sprite.h"
#include "game/state.h"

Ladder::Ladder(Sprite* sprite) {
    setLayer(1);
    allowTerrain(0);
    setSize(WIDTH, HEIGHT);
    body = new Animation(sprite, 1);
}

void Ladder::update(State* state) {
    body->update(state->clock->delta);
}

void Ladder::render(State* state) {
    if (state->camera->isVisible(getPosition())) {
        body->render(state->camera->translate(getPosition()));
    }
}

Ladder::~Ladder() {
    delete body;
}
