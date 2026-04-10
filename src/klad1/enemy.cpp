#include "klad1/enemy.h"
#include "klad1/klad1.h"
#include "klad1/player.h"
#include "klad1/type.h"
#include "klad1/unit_type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/position.h"
#include "game/scene.h"
#include "game/sprite.h"

#include <cmath>
#include "game/debug.h"

Enemy::Enemy(float x, float y, int spriteId) {
    setSize(19.8f, 13.84f);
    setLayer(1);
    allowTiles({TILE_BLANK});
    speed = 5.5f;
    this->spriteId = spriteId;
    spawnX = x;
    spawnY = y + 2.0f;
    setPosition(spawnX, spawnY);
    renderPosition = relativePosition(-5.94f, -6.95f, 31.68f, 21.78f);
}

void Enemy::prepare() {
    body = new Animation(scene->sprites[spriteId], IDLE);
}

void Enemy::scan() {
    inLadder = false;
    aboveLadder = false;
    nearBridge = 0;
    foundBridge = false;
    wrongBridge = false;

    if (world == nullptr || !isReady()) {
        return;
    }

    float feet = getY() + getHeight();

    scanUnits(2, 2, 2, 2, [&](Unit* unit) {
        switch (unit->getType()) {
            case UNIT_LADDER:
                if (intersects(unit)) {
                    if (
                        feet > unit->getY() && feet<unit->getY()+unit->getHeight()+LADDER_FALL_HOLD) {
                        inLadder = true;
                    }
                } else if (intersects(unit, 0, 0, 0, 2)) {
                    aboveLadder = true;
                }
                break;

            case UNIT_BRIDGE:
                if (!intersects(unit) && (intersects(unit, 2, 0, 0, 0) || intersects(unit, 0, 0, 2, 0))) {
                    foundBridge = true;
                    nearBridge =
                        (unit->getY() + unit->getHeight()) -
                        (getY() + getHeight());
                } else if (intersects(unit)) {
                    wrongBridge = true;
                }
                break;
        }
        return true;
    });
}

void Enemy::update(Context* context) {
    scan();

    float fallStep = (static_cast<float>(context->clock->delta) * speed) / 100.0f;
    bool fallingNow = !inLadder && !aboveLadder && canMove(0, fallStep);
    if (fallingNow) {
        float deltaX = 0;
        float deltaY = 0;
        move(context->clock->delta, 0, 1, deltaX, deltaY);
        falling += deltaY;
        body->play(IDLE);
        body->update(context->clock->delta);
        return;
    }

    falling = 0;

    Klad1* game = static_cast<Klad1*>(scene);
    Player* player = game->getPlayer();
    if (player == nullptr) {
        body->play(IDLE);
        body->update(context->clock->delta);
        return;
    }

    float moveX = 0;
    float moveY = 0;
    float desiredX = 0;
    float desiredY = 0;

    if (std::fabs(getX() - player->getX()) > 1.0f) {
        desiredX = getX() < player->getX() ? 1.0f : -1.0f;
    }

    if (std::fabs(getY() - player->getY()) > 1.0f) {
        if (inLadder) {
            desiredY = getY() < player->getY() ? 1.0f : -1.0f;
        } else if (aboveLadder && player->getY() > getY()) {
            desiredY = 1.0f;
        }
    }

    float straightX = (desiredX * static_cast<float>(context->clock->delta)) * (speed / 100.0f);
    float straightY = (desiredY * static_cast<float>(context->clock->delta)) * (speed / 100.0f);

    if (desiredX != 0 && canMove(straightX, 0)) {
        moveX = desiredX;
        if (foundBridge && nearBridge <= 1 && nearBridge > BRIDGE_SNAP_UP) {
            moveY = -1.0f;
        }
        facingRight = moveX > 0;
    } else if (desiredY != 0 && canMove(0, straightY)) {
        moveY = desiredY;
    }

    bool movedNow = false;
    deltaX = 0;
    deltaY = 0;
    if (moveX != 0 || moveY != 0) {
        movedNow = move(context->clock->delta, moveX, moveY, deltaX, deltaY);
    }

    if (deltaX == 0 && deltaY != 0 && inLadder) {
        body->play(CLIMB, 1.15f);
    } else if (deltaX < 0) {
        body->play(RUN_LEFT, 1.15f);
    } else if (deltaX > 0) {
        body->play(RUN_RIGHT, 1.15f);
    } else {
        body->play(IDLE);
    }

    body->update(context->clock->delta);
}

void Enemy::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition())) {
        // Klad1* game = static_cast<Klad1*>(scene);
        // Player* player = game->getPlayer();
        // print(getPosition(),
        //     "px", player != nullptr ? player->getX() : 0,
        //     "py", player != nullptr ? player->getY() : 0,
        //     "ex", getX(),
        //     "ey", getY(),
        //     "dx", deltaX,
        //     "dy", deltaY
        // );
        body->render(context->camera->translate(getRenderPosition()));
        // draw(getPosition());
    }
}

bool Enemy::canCrossUnit(Unit* target) const {
    switch (target->getType()) {
        case UNIT_LADDER:
            return true;

        case UNIT_BRIDGE:
            if (falling > BRIDGE_FALL_HOLD || nearBridge <= BRIDGE_SNAP_UP || wrongBridge) {
                return true;
            }
            break;

        case UNIT_BRICK:
            return false;
            if (position != nullptr) {
                SDL_FRect* self = position->getPosition();
                SDL_FRect* other = target->getPosition();
                if (self->x < other->x + other->w &&
                    self->x + self->w > other->x &&
                    self->y < other->y + other->h &&
                    self->y + self->h > other->y) {
                    return true;
                }
            }
            break;

        case UNIT_BULLET:
        case UNIT_PLAYER:
            return true;
    }

    return Unit::canCrossUnit(target);
}

int Enemy::getType() const {
    return UNIT_ENEMY;
}

void Enemy::respawn() {
    falling = 0;
    inLadder = false;
    aboveLadder = false;
    nearBridge = 0;
    foundBridge = false;
    wrongBridge = false;
    setPosition(spawnX, spawnY);
    body->play(IDLE);
}

bool Enemy::isFacingRight() const {
    return facingRight;
}

float Enemy::getFalling() const {
    return falling;
}

void Enemy::restore(float x, float y, bool facingRight, float falling) {
    setPosition(x, y);
    this->facingRight = facingRight;
    this->falling = falling;
}

Enemy::~Enemy() {
    delete body;
}
