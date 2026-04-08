// File: position.cpp
#include "game/position.h"

#include "game/camera.h"

// Positin with relative x, y, width, height
Position::Position(float x, float y, float width, float height,
                   float* parentX, float* parentY,
                   float* parentWidth, float* parentHeight) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->parentX = parentX;
    this->parentY = parentY;
    this->parentWidth = parentWidth;
    this->parentHeight = parentHeight;
    this->parentWidthRatio = *parentWidth / width;
    this->parentHeightRatio = *parentHeight / height;
}

// Position with relative x, y
Position::Position(float x, float y, float width, float height,
                    float* parentX, float* parentY) {
    // printf("Creating parent x: %d\n", x);
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->parentX = parentX;
    this->parentY = parentY;
    this->parentWidth = nullptr;
    this->parentHeight = nullptr;
}


// Position with no relative
Position::Position(float x, float y, float width, float height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->parentX = nullptr;
    this->parentY = nullptr;
    this->parentWidth = nullptr;
    this->parentHeight = nullptr;
}


// Position with no relative, just width and height
Position::Position(float width, float height) {
    this->width = width;
    this->height = height;
    this->parentX = nullptr;
    this->parentY = nullptr;
    this->parentWidth = nullptr;
    this->parentHeight = nullptr;
}

Position::Position() {
    this->parentX = nullptr;
    this->parentY = nullptr;
    this->parentWidth = nullptr;
    this->parentHeight = nullptr;
}

float Position::getX() {
    recalculateIfNeeded();
    return rect.x;
}

float Position::getY() {
    recalculateIfNeeded();
    return rect.y;
}

float Position::getWidth() {
    recalculateIfNeeded();
    return rect.w;
}

float Position::getHeight() {
    recalculateIfNeeded();
    return rect.h;
}

SDL_FRect* Position::getPosition() {
    recalculateIfNeeded();
    return &rect;
}

bool Position::intersects(SDL_FRect* target, float right, float top, float left, float bottom) {
    if (target == nullptr) {
        return false;
    }

    SDL_FRect* source = getPosition();

    float probeLeft = source->x - left;
    float probeTop = source->y - top;
    float probeRight = source->x + source->w + right;
    float probeBottom = source->y + source->h + bottom;

    return
        probeLeft < target->x + target->w &&
        probeRight > target->x &&
        probeTop < target->y + target->h &&
        probeBottom > target->y;
}

bool Position::intersects(Position* target, float right, float top, float left, float bottom) {
    if (target == nullptr) {
        return false;
    }

    return intersects(target->getPosition(), right, top, left, bottom);
}

float* Position::getXPtr() {
    return &rect.x;
}

float* Position::getYPtr() {
    return &rect.y;
}

float* Position::getWidthPtr() {
    return &rect.w;
}

float* Position::getHeightPtr() {
    return &rect.h;
}

void Position::addPosition(float x, float y) {
    if (parentX!=nullptr) {
        this->x += x;
        needsUpdate = true;
    } else {
        rect.x += x;
    }
    if (parentY!=nullptr) {
        this->y += y;
        needsUpdate = true;
    } else {
        rect.y += y;
    }
}

void Position::setPosition(float x, float y) {
    ready = true;
    if (parentX!=nullptr) {
        this->x = x;
        needsUpdate = true;
    } else {
        rect.x = x;
    }
    if (parentY!=nullptr) {
        this->y = y;
        needsUpdate = true;
    } else {
        rect.y = y;
    }
}

void Position::setSize(float width, float height) {

    if (parentWidth!=nullptr) {
        this->width = width;
        needsUpdate = true;
    } else {
        rect.w = width;
    };

    if (parentHeight!=nullptr) {
        this->height = height;
        needsUpdate = true;
    } else {
        rect.h = height;
    }

}

void Position::setX(float x) {
    if (parentX!=nullptr) {
        this->x = x;
        needsUpdate = true;
    } else {
        rect.x = x;
    }
}

void Position::setY(float y) {
    if (parentY!=nullptr) {
        this->y = y;
        needsUpdate = true;
    } else {
        rect.y = y;
    }
}

void Position::setHeight(float height) {
    if (parentHeight!=nullptr) {
        this->height = height;
        needsUpdate = true;
    } else {
        rect.h = height;
    }
}

void Position::setWidth(float width) {
    if (parentWidth!=nullptr) {
        this->width = width;
        needsUpdate = true;
    } else {
        rect.w = width;
    }
}

void Position::recalculateIfNeeded() {
    if (needsUpdate){
        if (parentX!=nullptr) {
            if (parentWidth!=nullptr) {
                rect.x = *parentX + (x * ((*parentWidth / width) / parentWidthRatio));
                rect.y = *parentY + (y * ((*parentHeight / height) / parentHeightRatio));
            } else {
                rect.x = *parentX + x;
                rect.y = *parentY + y;
                // printf("Updating relative x: %.2f + parentX: %.2f, y: %.2f + parentY: %.2f\n", x, *parentX, y, *parentY);
            }
        }
        if (parentWidth!=nullptr) {
            rect.w = width * ((*parentWidth / width) / parentWidthRatio);
            rect.h = height * ((*parentHeight / height) / parentHeightRatio);
        }
        needsUpdate = false;
    }
}

void Position::setRequiresUpdate() {
    needsUpdate = true;
}

bool Position::isReady() {
    return ready;
}


