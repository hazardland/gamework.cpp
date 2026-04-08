#include "game/mouse.h"

void Mouse::reset() {
    rightClick = false;
    leftClick = false;

    if (rightDragEnded) {
        rightDragEnded = false;
        rightDragStartX = -1;
        rightDragStartY = -1;
    }

    if (leftDragEnded) {
        leftDragEnded = false;
        leftDragStartX = -1;
        leftDragStartY = -1;
    }
}

void Mouse::move(float x, float y) {
    this->x = x;
    this->y = y;

    if (!rightDragActive
        && rightDragStartX > -1
        && (rightDragStartX != x || rightDragStartY != y)) {
        rightDragActive = true;
    }

    if (!leftDragActive
        && leftDragStartX > -1
        && (leftDragStartX != x || leftDragStartY != y)) {
        leftDragActive = true;
    }
}

void Mouse::down(int button) {
    if (button == SDL_BUTTON_RIGHT) {
        rightDragStartX = x;
        rightDragStartY = y;
    } else if (button == SDL_BUTTON_LEFT) {
        leftDragStartX = x;
        leftDragStartY = y;
    }
}

void Mouse::up(int button) {
    if (button == SDL_BUTTON_RIGHT) {
        if (rightDragActive) {
            rightDragEnded = true;
            rightDragActive = false;
        } else {
            rightClick = true;
            rightDragStartX = -1;
            rightDragStartY = -1;
        }
    } else if (button == SDL_BUTTON_LEFT) {
        if (leftDragActive) {
            leftDragEnded = true;
            leftDragActive = false;
        } else {
            leftClick = true;
            leftDragStartX = -1;
            leftDragStartY = -1;
        }
    }
}

bool Mouse::inside(SDL_FRect* position) {
    if (x>=position->x &&
        y>=position->y &&
        x<=position->x+position->w &&
        y<=position->y+position->h) {
            return true;
        }
    return false;
}


