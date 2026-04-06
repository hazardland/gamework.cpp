#ifndef KLAD1_BRICK_H
#define KLAD1_BRICK_H

#include "game/unit.h"

class Animation;
class Sprite;
class State;

class Brick : public Unit {
public:
    Brick(Sprite* sprite);

    void update(State* state) override;
    void render(State* state) override;

    ~Brick();

private:
    static constexpr int WIDTH = 32;
    static constexpr int HEIGHT = 22;
    Animation* body = nullptr;
};

#endif // KLAD1_BRICK_H
