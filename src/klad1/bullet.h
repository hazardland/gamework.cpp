#ifndef KLAD1_BULLET_H
#define KLAD1_BULLET_H

#include "game/unit.h"

class Animation;
class Context;

class Bullet : public Unit {
public:
    Bullet(float x, float y, bool facingRight);

    void update(Context* context) override;
    void render(Context* context) override;
    bool isDead() const;
    int getType() const override;

    ~Bullet() override;

private:
    Animation* body = nullptr;
    bool dead = false;
    bool dying = false;
    float direction = 1.0f;

    float findWallHitX(float startFront, float endFront);

protected:
    void prepare() override;
};

#endif // KLAD1_BULLET_H
