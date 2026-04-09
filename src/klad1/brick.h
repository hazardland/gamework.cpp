#ifndef KLAD1_BRICK_H
#define KLAD1_BRICK_H

#include <SDL3/SDL_timer.h>

#include "game/unit.h"

class Animation;
class Sprite;
class Context;

class Brick : public Unit {
public:
    enum class HitSide {
        Left,
        Right
    };

    Brick(Sprite* sprite, float x, float y);

    void update(Context* context) override;
    void render(Context* context) override;
    int getType() const override;
    bool isSolid() const override;
    void hit(HitSide side);
    float getHitX(HitSide side);
    bool isDestroyed() const;

    ~Brick();

private:
    static constexpr int FULL = 1;
    static constexpr int DAMAGE_LEFT_1 = 2;
    static constexpr int DAMAGE_LEFT_2 = 3;
    static constexpr int DAMAGE_LEFT_3 = 4;
    static constexpr int REGEN_1 = 5;
    static constexpr int REGEN_2 = 6;
    static constexpr int REGEN_3 = 7;
    static constexpr int DAMAGE_RIGHT_1 = 8;
    static constexpr int DAMAGE_RIGHT_2 = 9;
    static constexpr int DAMAGE_RIGHT_3 = 10;
    static constexpr int DAMAGE_BOTH_1 = 11;

    Animation* body = nullptr;
    int damagedLeft = 0;
    int damagedRight = 0;
    Uint64 destroyedAt = 0;
    bool regenerating = false;

    int getDamagedCount() const;
    int getIntactCount() const;
    float getQuarterWidth();
    int getClipId() const;
};

#endif // KLAD1_BRICK_H


