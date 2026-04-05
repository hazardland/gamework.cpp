#include "game/camera.h"
#include "game/image.h"
#include "war2/farm.h"

Farm::Farm(Sprite* sprite) {
    setLayer(1);
    setSize(96, 96);
    allowTerrain(2);
    setColor ({255, 0, 0, 255});
    renderPosition = createChildPosition(0, 0, 96, 96);
    body = sprite;
}

void Farm::render(State* state) {
    Camera* camera = state->camera;
    if (camera->isVisible(getRenderPosition())) {
        // drawPosition(state);
        body->image->render(body->getRect(), camera->translate(getRenderPosition()));
    }
}
