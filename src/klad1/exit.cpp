#include "klad1/exit.h"

#include "game/camera.h"
#include "game/context.h"
#include "game/debug.h"

Exit::Exit(float x, float y) {
    setSize(32, 22);
    setPosition(x, y);
}

void Exit::render(Context* context) {
    if (context->camera->isVisible(getPosition())) {
        // draw(getPosition(), {255, 210, 70, 255});
    }
}
