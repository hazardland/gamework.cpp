#include "klad1/gold.h"
#include "klad1/klad1.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/scene.h"
#include "game/sprite.h"

Gold::Gold(float x, float y, int index) {
    setSize(32, 22);
    setPosition(x, y);
    this->index = index;
}

void Gold::prepare() {
    body = new Animation(scene->sprites[Klad1::SPRITE_GOLD], 1);
}

void Gold::update(Context* context) {
    body->update(context->clock->delta);
}

void Gold::render(Context* context) {
    if (!collected && context->camera->isVisible(getPosition())) {
        body->render(context->camera->translate(getPosition()));
    }
}

int Gold::getIndex() const {
    return index;
}

bool Gold::isCollected() const {
    return collected;
}

void Gold::collect() {
    collected = true;
}

void Gold::setCollected(bool collected) {
    this->collected = collected;
}

Gold::~Gold() {
    delete body;
}
