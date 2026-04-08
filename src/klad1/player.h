#ifndef KLAD1_PLAYER_H
#define KLAD1_PLAYER_H

#include "game/unit.h"
#include "game/cooldown.h"

class Animation;
class Sprite;
class Context;
typedef struct TTF_Font TTF_Font;

class Player : public Unit {
public:
    Player(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    bool canCrossUnit(Unit* target) const override;
    void respawn(float x, float y);
    bool isFalling() const;
    void shoot();
    bool isFacingRight() const;
    float getBulletX() const;
    float getBulletY() const;

    ~Player();

private:
    static constexpr int IDLE = 1;
    static constexpr int CLIMB = 2;
    static constexpr int RUN_LEFT = 3;
    static constexpr int RUN_RIGHT = 4;
    static constexpr int SHOOT_RIGHT = 5;
    static constexpr int SHOOT_LEFT = 6;
    
    Animation* body = nullptr;
    float falling = 0;
    bool inLadder = false;
    bool aboveLadder = false;
    float nearBridge = 0;
    float nearLadder = 0;
    bool foundBridge = false;
    bool wrongBridge = false;
    bool facingRight = true;
    Cooldown shootCooldown = Cooldown(120);

    void scan();
};

#endif // KLAD1_PLAYER_H
