#include "game/object.h"


#include "game/context.h"
#include "game/position.h"
#include "game/scene.h"


uint32_t Object::count = 0;

Object::Object() : position(new Position()) {}

uint32_t Object::getId() {
    if (id == 0) {
        Object::count++;
        id = Object::count;
    }
    return id;
}

void Object::addPosition(float x, float y) {
    position->addPosition(x, y);
    updateChildPositions();
}

void Object::setPosition(float x, float y) {
    position->setPosition(x, y);
    updateChildPositions();
}

void Object::setSize(float width, float height) {
    position->setSize(width, height);
}

void Object::setX(float x) {
    position->setX(x);
    updateChildPositions();
}

void Object::setY(float y) {
    position->setY(y);
    updateChildPositions();
}

void Object::setHeight(float height) {
    position->setHeight(height);
    updateChildPositions();
}

void Object::setWidth(float width) {
    position->setWidth(width);
    updateChildPositions();
}

float Object::getX() {
    return position->getX();
}

float Object::getY() {
    return position->getY();
}

float Object::getHeight() {
    return position->getHeight();
}

float Object::getWidth() {
    return position->getWidth();
}

bool Object::isReady() {
    return position->isReady();
}

SDL_FRect* Object::getPosition() {
    return position->getPosition();
}

bool Object::intersects(SDL_FRect* target, float right, float top, float left, float bottom) {
    return position->intersects(target, right, top, left, bottom);
}

bool Object::intersects(Position* target, float right, float top, float left, float bottom) {
    return position->intersects(target, right, top, left, bottom);
}

bool Object::intersects(Object* target, float right, float top, float left, float bottom) {
    if (target == nullptr) {
        return false;
    }

    return position->intersects(target->getPosition(), right, top, left, bottom);
}

Position* Object::relativePosition(float x, float y, float width, float height) {
    Position* pos =  new Position(
                        x, y,
                        width, height,
                        position->getXPtr(),
                        position->getYPtr(),
                        position->getWidthPtr(),
                        position->getHeightPtr()
                    );
    childPositions.push_back(pos);
    return pos;
}

Position* Object::relativePosition(float x, float y) {
    Position* pos =  new Position(
                        x, y,
                        0, 0,
                        position->getXPtr(),
                        position->getYPtr()
                    );
    childPositions.push_back(pos);
    return pos;
}

bool Object::isVisible(Context* context) {
    return true;
}

void Object::update(Context* context) {
}

void Object::render(Context* context) {
}

void Object::updateChildPositions() {
    for (auto pos : childPositions) {
        pos->setRequiresUpdate();
    }
}

Object::~Object() {
    delete position;
    for (auto pos : childPositions) {
        delete pos;
    }
}


