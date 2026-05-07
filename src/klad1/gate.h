#ifndef KLAD1_GATE_H
#define KLAD1_GATE_H

#include "game/unit.h"

class Animation;
class Context;

class Gate : public Unit {
public:
    Gate(float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    bool isSolid() const override;
    void open();
    bool isOpen() const;
    void setOpen(bool opened);
    int getType() const override;

    ~Gate() override;

private:
    Animation* body = nullptr;
    bool opened = false;

protected:
    void prepare() override;
};

#endif // KLAD1_GATE_H
