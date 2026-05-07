#include "klad1/key.h"
#include "klad1/klad1.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/scene.h"
#include "game/sprite.h"

Key::Key(float x, float y) {
    setSize(32, 22);
    setPosition(x, y);
}

void Key::prepare() {
    body = new Animation(scene->sprites[Klad1::SPRITE_KEY], 1);
}

void Key::update(Context* context) {
    body->update(context->clock->delta);
}

void Key::render(Context* context) {
    if (context->camera->isVisible(getPosition())) {
        body->render(context->camera->translate(getPosition()));
    }
}

Key::~Key() {
    delete body;
}
