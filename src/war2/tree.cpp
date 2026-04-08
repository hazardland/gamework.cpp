#include "game/camera.h"
#include "game/image.h"
#include "war2/tree.h"

Tree::Tree(Sprite* sprite) {
    setLayer(1);
    setSize(20, 20);
    allowTerrain(2);
    setColor ({34, 139, 34, 255});
    renderPosition = relativePosition(-22, -76, 64, 96);
    body = sprite;
}

void Tree::render(State* state) {
    Camera* camera = state->camera;
    if (camera->isVisible(getRenderPosition())) {
        body->image->render(body->getRect(), camera->translate(getRenderPosition()));
        drawPosition(state);
    }
}
