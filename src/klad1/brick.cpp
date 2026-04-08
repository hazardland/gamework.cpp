#include "klad1/brick.h"
#include "klad1/type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/sprite.h"
#include "game/context.h"

Brick::Brick(Sprite* sprite, float x, float y) {
    setLayer(1);
    allowTile(TILE_BLANK);
    setSize(32, 22);
    setPosition(x, y);
    body = new Animation(sprite, 1);
}

void Brick::update(Context* context) {
    body->update(context->clock->delta);
}

void Brick::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition())) {
        body->render(context->camera->translate(getRenderPosition()));
    }
}

Brick::~Brick() {
    delete body;
}


