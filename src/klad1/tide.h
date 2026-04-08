#ifndef KLAD1_TIDE_H
#define KLAD1_TIDE_H

#include "game/object.h"

class Animation;
class Sprite;
class Context;

class Tide : public Object {
public:
    Tide(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;

    ~Tide() override;

private:
    Animation* body = nullptr;
};

#endif // KLAD1_TIDE_H
