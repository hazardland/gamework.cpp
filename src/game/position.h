// File: position.h

#ifndef GAME_POSITION_H
#define GAME_POSITION_H

#include <SDL3_image/SDL_image.h>

class Position {
public:
    // If parent pointers are passed, this position becomes relative to the parent.
    // Child positions work by storing pointers to the parent's rect fields.
    // Example: Object::relativePosition(...) links a render/select box to the main position.
    Position(float x, float y, float width, float height,
             float* parentX, float* parentY,
             float* parentWidth, float* parentHeight);

    Position(float x, float y, float width, float height,
             float* parentX, float* parentY);

    Position(float x, float y, float width, float height);
    Position(float width, float height);
    Position();

    ~Position() = default;

    float getX();
    float getY();
    float getWidth();
    float getHeight();
    void addPosition(float x, float y);
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setX(float x);
    void setY(float y);
    void setHeight(float height);
    void setWidth(float width);

    void setRequiresUpdate();
    SDL_FRect* getPosition();
    bool intersects(SDL_FRect* target, float right = 0, float top = 0, float left = 0, float bottom = 0);
    bool intersects(Position* target, float right = 0, float top = 0, float left = 0, float bottom = 0);
    float* getXPtr();
    float* getYPtr();
    float* getWidthPtr();
    float* getHeightPtr();
    bool isReady();

    SDL_FRect rect;

private:
    void recalculateIfNeeded();
    bool parent = false;
    bool needsUpdate = true;
    float* parentX;
    float* parentY;
    float* parentWidth;
    float* parentHeight;
    float parentWidthRatio;
    float parentHeightRatio;
    float x;
    float y;
    float width;
    float height;
    bool ready = false;
};

#endif // GAME_POSITION_H


