#ifndef KLAD1_ENEMY_H
#define KLAD1_ENEMY_H

#include "game/unit.h"

class Animation;
class Context;

class Enemy : public Unit {
public:
    Enemy(float x, float y, int spriteId);

    void update(Context* context) override;
    void render(Context* context) override;
    bool canCrossUnit(Unit* target) const override;
    int getType() const override;
    void respawn();
    bool isFacingRight() const;
    float getFalling() const;
    void restore(float x, float y, bool facingRight, float falling);

    ~Enemy();

private:
    static constexpr int IDLE = 1;
    static constexpr int CLIMB = 2;
    static constexpr int RUN_LEFT = 3;
    static constexpr int RUN_RIGHT = 4;
    static constexpr int BRIDGE_SNAP_UP = -6;
    static constexpr int BRIDGE_FALL_HOLD = 6;
    static constexpr int LADDER_FALL_HOLD = 7;

    Animation* body = nullptr;
    int spriteId = 0;
    float spawnX = 0;
    float spawnY = 0;
    float falling = 0;
    bool inLadder = false;
    bool aboveLadder = false;
    float nearBridge = 0;
    bool foundBridge = false;
    bool wrongBridge = false;
    bool facingRight = true;

    void scan();

protected:
    void prepare() override;
    float deltaX;
    float deltaY;
};

#endif // KLAD1_ENEMY_H
