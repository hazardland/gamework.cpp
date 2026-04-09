#ifndef KLAD1_GATE_H
#define KLAD1_GATE_H

#include "game/unit.h"

class Animation;
class Sprite;
class Context;

class Gate : public Unit {
public:
    Gate(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    bool isSolid() const override;
    void open();
    bool isOpen() const;
    int getType() const override;

    ~Gate() override;

private:
    Animation* body = nullptr;
    bool opened = false;
};

#endif // KLAD1_GATE_H
