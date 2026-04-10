#ifndef KLAD1_DOOR_H
#define KLAD1_DOOR_H

#include "game/unit.h"

class Animation;
class Context;

class Door : public Unit {
public:
    Door(float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    bool isSolid() const override;
    void open();
    bool isOpen() const;
    void setOpen(bool opened);
    int getType() const override;

    ~Door() override;

private:
    Animation* body = nullptr;
    bool opened = false;

protected:
    void prepare() override;
};

#endif // KLAD1_DOOR_H
