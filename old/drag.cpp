#include "game/drag.h"

#include <game/context.h>
#include <game/input.h>


Drag::Drag(SDL_FRect* position, bool dragLeft, bool dragRight) {
    this->position = position;
    this->dragLeft = dragLeft;
    this->dragRight = dragRight;
}

void Drag::update(Context* context) {
    if (enabled) {
        Mouse* mouse = context->input->mouse;
        if (dragRight && !dragging && mouse->rightDragActive && mouse->inside(position)) {
            dragging = true;
            dragStartX = mouse->x - position->x;
            dragStartY = mouse->y - position->y;
        }

        if (dragLeft && !dragging && mouse->leftDragActive && mouse->inside(position)) {
            dragging = true;
            dragStartX = mouse->x - position->x;
            dragStartY = mouse->y - position->y;
        }

        if (dragging) {
            position->x = mouse->x - dragStartX;
            position->y = mouse->y - dragStartY;
        }

        if (mouse->rightDragEnded && dragging) {
            dragging = false;
        }
    }
}

void Drag::disable() {
    dragging = false;
    enabled = false;
}

void Drag::enable() {
    enabled = true;
}


