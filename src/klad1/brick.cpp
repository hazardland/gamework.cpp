#include "klad1/brick.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/sprite.h"
#include "game/state.h"

Brick::Brick(Sprite* sprite) {
    setLayer(1);
    allowTerrain(0);
    setSize(32, 22);
    body = new Animation(sprite, 1);
}

void Brick::update(State* state) {
    body->update(state->clock->delta);
}

void Brick::render(State* state) {
    if (state->camera->isVisible(getPosition())) {
        body->render(state->camera->translate(getPosition()));
    }
}

Brick::~Brick() {
    delete body;
}
