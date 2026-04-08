#include "game/context.h"

#include "game/camera.h"
#include "game/clock.h"
#include "game/input.h"
#include "game/screen.h"

Context::Context() {
    clock = new Clock();
    input = new Input();
    camera = new Camera();
    screen = new Screen();
}

Context::~Context() {
    delete clock;
    delete input;
    delete camera;
    delete screen;
}

