#include "klad1/gate.h"
#include "klad1/klad1.h"
#include "klad1/type.h"
#include "klad1/unit_type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/scene.h"
#include "game/sprite.h"

Gate::Gate(float x, float y) {
    setLayer(Klad1::LAYER_WORLD);
    allowTile(TILE_BLANK);
    setSize(32, 22);
    setPosition(x, y);
}

void Gate::prepare() {
    body = new Animation(scene->sprites[Klad1::SPRITE_DOOR], 1);
}

void Gate::update(Context* context) {
    body->play(opened ? 2 : 1);
    body->update(context->clock->delta);
}

void Gate::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition())) {
        body->render(context->camera->translate(getRenderPosition()));
    }
}

bool Gate::isSolid() const {
    return !opened;
}

void Gate::open() {
    opened = true;
}

bool Gate::isOpen() const {
    return opened;
}

int Gate::getType() const {
    return UNIT_GATE;
}

Gate::~Gate() {
    delete body;
}
