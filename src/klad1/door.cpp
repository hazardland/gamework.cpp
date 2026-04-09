#include "klad1/door.h"
#include "klad1/klad1.h"
#include "klad1/type.h"
#include "klad1/unit_type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/scene.h"
#include "game/sprite.h"

Door::Door(float x, float y) {
    setLayer(Klad1::LAYER_WORLD);
    allowTile(TILE_BLANK);
    setSize(32, 22);
    setPosition(x, y);
}

void Door::prepare() {
    body = new Animation(scene->sprites[Klad1::SPRITE_DOOR], 1);
}

void Door::update(Context* context) {
    body->play(opened ? 2 : 1);
    body->update(context->clock->delta);
}

void Door::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition())) {
        body->render(context->camera->translate(getRenderPosition()));
    }
}

bool Door::isSolid() const {
    return !opened;
}

void Door::open() {
    opened = true;
}

bool Door::isOpen() const {
    return opened;
}

int Door::getType() const {
    return UNIT_DOOR;
}

Door::~Door() {
    delete body;
}
