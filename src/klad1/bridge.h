#ifndef KLAD1_BRIDGE_H
#define KLAD1_BRIDGE_H

#include "game/unit.h"

class Animation;
class Sprite;
class State;

class Bridge : public Unit {
public:
    Bridge(Sprite* sprite);

    void update(State* state) override;
    void render(State* state) override;
    int getKind() const override;

    ~Bridge();

private:
    static constexpr int WIDTH = 32;
    static constexpr int HEIGHT = 1;

    Animation* body = nullptr;
};

#endif // KLAD1_BRIDGE_H
