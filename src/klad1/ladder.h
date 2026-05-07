#ifndef KLAD1_LADDER_H
#define KLAD1_LADDER_H

#include "game/unit.h"

class Animation;
class Context;

class Ladder : public Unit {
public:
    Ladder(float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    int getType() const override;

    ~Ladder();

private:
    Animation* body = nullptr;

protected:
    void prepare() override;
};

#endif // KLAD1_LADDER_H


