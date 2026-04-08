#ifndef KLAD1_LADDER_H
#define KLAD1_LADDER_H

#include "game/unit.h"

class Animation;
class Sprite;
class Context;

class Ladder : public Unit {
public:
    Ladder(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    int getType() const override;

    ~Ladder();

private:
    Animation* body = nullptr;
};

#endif // KLAD1_LADDER_H


