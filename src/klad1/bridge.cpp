#include "klad1/bridge.h"
#include "klad1/kind.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/sprite.h"
#include "game/state.h"

Bridge::Bridge(Sprite* sprite) {
    setLayer(0);
    allowTerrain(0);
    setSize(WIDTH, 1);
    body = new Animation(sprite, 1);
    renderPosition = createChildPosition(0, 0, 32, 22);
}

void Bridge::update(State* state) {
    body->update(state->clock->delta);
}

void Bridge::render(State* state) {
    if (state->camera->isVisible(getRenderPosition())) {
        drawPosition(state);
        body->render(state->camera->translate(getRenderPosition()));
    }
}

int Bridge::getKind() const {
    return KIND_BRIDGE;
}

Bridge::~Bridge() {
    delete body;
}
