#include "klad1/tide.h"
#include "klad1/klad1.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/scene.h"
#include "game/sprite.h"

Tide::Tide(float x, float y) {
    setSize(32, 22);
    setPosition(x, y);
}

void Tide::prepare() {
    body = new Animation(scene->sprites[Klad1::SPRITE_TIDE], 1);
}

void Tide::update(Context* context) {
    body->update(context->clock->delta);
}

void Tide::render(Context* context) {
    if (context->camera->isVisible(getPosition())) {
        body->render(context->camera->translate(getPosition()));
    }
}

Tide::~Tide() {
    delete body;
}
