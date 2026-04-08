#ifndef KLAD1_BRIDGE_H
#define KLAD1_BRIDGE_H

#include "game/unit.h"

class Animation;
class Sprite;
class Context;

class Bridge : public Unit {
public:
    Bridge(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    int getType() const override;

    ~Bridge();

private:
    Animation* body = nullptr;
};

#endif // KLAD1_BRIDGE_H


