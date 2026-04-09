#include "klad1/brick.h"
#include "klad1/type.h"
#include "klad1/unit_type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/sprite.h"
#include "game/context.h"

Brick::Brick(Sprite* sprite, float x, float y) {
    setLayer(1);
    allowTile(TILE_BLANK);
    setSize(32, 22);
    setPosition(x, y);
    body = new Animation(sprite, FULL);
}

int Brick::getDamagedCount() const {
    return damagedLeft + damagedRight;
}

int Brick::getIntactCount() const {
    return 4 - getDamagedCount();
}

float Brick::getQuarterWidth() {
    return getWidth() / 4.0f;
}

bool Brick::isDestroyed() const {
    return getIntactCount() <= 0;
}

bool Brick::isSolid() const {
    return !regenerating && getIntactCount() > 0;
}

float Brick::getHitX(HitSide side) {
    if (!isSolid()) {
        return -1.0f;
    }

    float quarterWidth = getQuarterWidth();
    if (side == HitSide::Left) {
        return getX() + (damagedLeft * quarterWidth);
    }

    return getX() + getWidth() - (damagedRight * quarterWidth);
}

void Brick::hit(HitSide side) {
    if (!isSolid()) {
        return;
    }

    if (side == HitSide::Left) {
        damagedLeft++;
    } else {
        damagedRight++;
    }

    if (getDamagedCount() > 4) {
        if (side == HitSide::Left) {
            damagedLeft--;
        } else {
            damagedRight--;
        }
        return;
    }

    if (isDestroyed()) {
        regenerating = true;
        destroyedAt = SDL_GetTicks();
    }
}

int Brick::getClipId() const {
    if (regenerating) {
        Uint64 elapsed = SDL_GetTicks() - destroyedAt;
        int stage = static_cast<int>(elapsed / 6000);

        if (stage <= 0) {
            return 0;
        }
        if (stage == 1) {
            return REGEN_1;
        }
        if (stage == 2) {
            return REGEN_2;
        }
        if (stage == 3) {
            return REGEN_3;
        }

        return FULL;
    }

    if (damagedLeft == 0 && damagedRight == 0) {
        return FULL;
    }

    if (damagedLeft == 1 && damagedRight == 0) {
        return DAMAGE_LEFT_1;
    }
    if (damagedLeft == 2 && damagedRight == 0) {
        return DAMAGE_LEFT_2;
    }
    if (damagedLeft == 3 && damagedRight == 0) {
        return DAMAGE_LEFT_3;
    }

    if (damagedLeft == 0 && damagedRight == 1) {
        return DAMAGE_RIGHT_1;
    }
    if (damagedLeft == 0 && damagedRight == 2) {
        return DAMAGE_RIGHT_2;
    }
    if (damagedLeft == 0 && damagedRight == 3) {
        return DAMAGE_RIGHT_3;
    }

    if (damagedLeft == 1 && damagedRight == 1) {
        return DAMAGE_BOTH_1;
    }

    return 0;
}

void Brick::update(Context* context) {
    if (regenerating) {
        Uint64 elapsed = SDL_GetTicks() - destroyedAt;
        if (elapsed >= 24000) {
            damagedLeft = 0;
            damagedRight = 0;
            regenerating = false;
        }
    }

    int clipId = getClipId();
    if (clipId > 0) {
        body->play(clipId);
        body->update(context->clock->delta);
    }
}

void Brick::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition()) && getClipId() > 0) {
        body->render(context->camera->translate(getRenderPosition()));
    }
}

int Brick::getType() const {
    return UNIT_BRICK;
}

Brick::~Brick() {
    delete body;
}


