#ifndef KLAD1_GOLD_H
#define KLAD1_GOLD_H

#include "game/object.h"

class Animation;
class Sprite;
class Context;

class Gold : public Object {
public:
    Gold(Sprite* sprite, float x, float y, int index);

    void update(Context* context) override;
    void render(Context* context) override;
    int getIndex() const;
    bool isCollected() const;
    void collect();

    ~Gold() override;

private:
    Animation* body = nullptr;
    int index = -1;
    bool collected = false;
};

#endif // KLAD1_GOLD_H
