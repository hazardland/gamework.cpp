#ifndef KLAD1_GOLD_H
#define KLAD1_GOLD_H

#include "game/object.h"

class Animation;
class Context;

class Gold : public Object {
public:
    Gold(float x, float y, int index);

    void update(Context* context) override;
    void render(Context* context) override;
    int getIndex() const;
    bool isCollected() const;
    void collect();
    void setCollected(bool collected);

    ~Gold() override;

private:
    Animation* body = nullptr;
    int index = -1;
    bool collected = false;

protected:
    void prepare() override;
};

#endif // KLAD1_GOLD_H
