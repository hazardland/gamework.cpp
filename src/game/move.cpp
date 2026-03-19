#include <cmath>

#include "game/move.h"
#include "game/unit.h"
#include "game/state.h"
#include "game/clock.h"
#include <SDL3/SDL.h>


Move::Move(Unit* unit, float travelX, float travelY) {
    this->unit = unit;
    this->travelX = travelX;
    this->travelY = travelY;
    printf("--------------------------------\n");
}
Move::~Move() {}

bool Move::update(State* state) {
    Uint64 deltaTime = state->clock->delta;

    // Stop early if we’re already close enough
    if ((travelX > -0.05f && travelX < 0.05f) &&
        (travelY > -0.05f && travelY < 0.05f)) {
        return false;
    }

    float dirX = travelX < 0 ? -1.0f : (travelX > 0 ? 1.0f : 0.0f);
    float dirY = travelY < 0 ? -1.0f : (travelY > 0 ? 1.0f : 0.0f);

    float deltaX = 0, deltaY = 0;
    bool moved = unit->move(deltaTime, dirX, dirY, deltaX, deltaY);

    printf("delta: %f, %f\n", deltaX, deltaY);
    printf("travel: %f, %f\n", travelX, travelY);


    // Clamp delta to remaining travel to avoid overshoot
    if (std::abs(deltaX) > std::abs(travelX)) {
        deltaX = travelX;
    }
    if (std::abs(deltaY) > std::abs(travelY)) {
        deltaY = travelY;
    }
    

    travelX -= deltaX;
    travelY -= deltaY;

    return moved;
}

// bool Move::update(State* state) {
//     Uint64 deltaTime = state->clock->delta;

//     // Stop early if we’re already close enough
//     if ((travelX > -0.05f && travelX < 0.05f) &&
//         (travelY > -0.05f && travelY < 0.05f)) {
//         return false;
//     }

//     float dirX = travelX < 0 ? -1.0f : (travelX > 0 ? 1.0f : 0.0f);
//     float dirY = travelY < 0 ? -1.0f : (travelY > 0 ? 1.0f : 0.0f);

//     float startX = unit->getX();
//     float startY = unit->getY();

//     // Move based on direction
//     bool moved = unit->move(deltaTime, dirX, dirY, /* dummy */ *(new float), *(new float));

//     float endX = unit->getX();
//     float endY = unit->getY();

//     float deltaX = endX - startX;
//     float deltaY = endY - startY;

//     // Clamp movement to avoid overshoot
//     if ((travelX > 0 && deltaX > travelX) || (travelX < 0 && deltaX < travelX)) {
//         deltaX = travelX;
//     }
//     if ((travelY > 0 && deltaY > travelY) || (travelY < 0 && deltaY < travelY)) {
//         deltaY = travelY;
//     }

//     travelX -= deltaX;
//     travelY -= deltaY;

//     printf("delta: %f, %f\n", deltaX, deltaY);
//     printf("travel: %f, %f\n", travelX, travelY);

//     return moved;
// }


void Move::render (State* state) {
    // printf("travel: %f, %f\n", travelX, travelY);
}

std::vector<Job*> Move::finish() {
    return {
        // new Move(unit, (float)(rand() % 1001 - 500), (float)(rand() % 1001 - 500))
    };
}