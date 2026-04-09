#ifndef KLAD1_KEY_H
#define KLAD1_KEY_H

#include "game/object.h"

class Animation;
class Context;

class Key : public Object {
public:
    Key(float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    ~Key() override;

private:
    Animation* body = nullptr;

protected:
    void prepare() override;
};

#endif // KLAD1_KEY_H
