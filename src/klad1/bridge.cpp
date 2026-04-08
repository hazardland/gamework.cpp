#include "klad1/bridge.h"
#include "klad1/kind.h"
#include "klad1/klad1.h"
#include "klad1/type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/sprite.h"
#include "game/context.h"
#include "game/debug.h"

Bridge::Bridge(Sprite* sprite, float x, float y) {
    setLayer(Klad1::LAYER_WORLD);
    allowTile(TILE_BLANK);
    setSize(32, 1);
    setPosition(x, y);
    body = new Animation(sprite, 1);
    renderPosition = relativePosition(0, -2, 32, 22);
}

void Bridge::update(Context* context) {
    body->update(context->clock->delta);
}

void Bridge::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition())) {
        // draw(getPosition());
        body->render(context->camera->translate(getRenderPosition()));
    }
}

int Bridge::getType() const {
    return UNIT_BRIDGE;
}

Bridge::~Bridge() {
    delete body;
}
