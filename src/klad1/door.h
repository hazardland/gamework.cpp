#ifndef KLAD1_DOOR_H
#define KLAD1_DOOR_H

#include "game/object.h"

class Animation;
class Sprite;
class Context;

class Door : public Object {
public:
    Door(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;

    ~Door() override;

private:
    Animation* body = nullptr;
};

#endif // KLAD1_DOOR_H
