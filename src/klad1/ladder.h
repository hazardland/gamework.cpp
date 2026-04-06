#ifndef KLAD1_LADDER_H
#define KLAD1_LADDER_H

#include "game/unit.h"

class Animation;
class Sprite;
class State;

class Ladder : public Unit {
public:
    Ladder(Sprite* sprite);

    void update(State* state) override;
    void render(State* state) override;

    ~Ladder();

private:
    static constexpr int WIDTH = 32;
    static constexpr int HEIGHT = 22;
    Animation* body = nullptr;
};

#endif // KLAD1_LADDER_H
