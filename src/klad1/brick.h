#ifndef KLAD1_BRICK_H
#define KLAD1_BRICK_H

#include "game/unit.h"

class Animation;
class Sprite;
class Context;

class Brick : public Unit {
public:
    Brick(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;

    ~Brick();

private:
    Animation* body = nullptr;
};

#endif // KLAD1_BRICK_H


