#include <cstdlib> // for rand()

#include "game/camera.h"
#include "game/image.h"
#include "game/clock.h"
#include "game/keyboard.h"
#include "game/input.h"
#include "game/input.h"
#include "game/move.h"

#include "war2/critter.h"
#include "war2/enum.h"

// Constructor
Critter::Critter(Sprite* sprite, TTF_Font* font) {
    setLayer(1);
    setSize(32, 32);
    allowTerrains({1,2});
    renderPosition = relativePosition(0, 0, 32, 32);
    body = new Animation(sprite, DOWN);
    setColor ({165, 42, 42});
    debug = new Text(font, relativePosition(-10, -40));
    speed = 5;

    // travelX = (std::rand() % 51) - 25; // -25 to 25
    // travelY = (std::rand() % 51) - 25;

    addJob(new Move(this, (float) (rand() % 1001 - 500), (float) (rand() % 1001 - 500)));
}

void Critter::rotate(float axisX, float axisY) {
    if (axisX != 0 || axisY != 0) {
        if (axisX != 0) {
            faceX = (axisX < 0) ? LEFT : RIGHT;
        } else {
            faceX = 0;
        }
        if (axisY != 0) {
            faceY = (axisY < 0) ? UP : DOWN;
        } else {
            faceY = 0;
        }
    }
}

void Critter::onJobFinished(Job* job) {
    if (job->getType()==Move::TYPE) {
        //addJob(new Move(this, (float) (rand() % 1001 - 500), (float) (rand() % 1001 - 500)));
    }
}

void Critter::update(State* state) {

    updateJobs(state);
    body->play(faceX + faceY);
    body->update(state->clock->delta);
}



// Render function
void Critter::render(State* state) {
    Camera* camera = state->camera;
    if (camera->isVisible(getRenderPosition())) {
        body->render(camera->translate(getRenderPosition()));
        debug->render(state);
        renderJobs(state);
        // drawPosition(state);
    }
}

