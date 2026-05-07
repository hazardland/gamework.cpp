#include "klad1/player.h"
#include "klad1/klad1.h"
#include "klad1/brick.h"
#include "klad1/type.h"
#include "klad1/unit_type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/cell.h"
#include "game/clock.h"
#include "game/debug.h"
#include "game/input.h"
#include "game/world.h"
#include "game/position.h"
#include "game/scene.h"
#include "game/sound.h"
#include "game/sprite.h"
#include "game/context.h"

Player::Player(float x, float y) {
    setSize(19.8f, 13.84f);
    setLayer(1);
    allowTiles({TILE_BLANK});
    speed = 11.f;
    setPosition(x, y + 2.0f);
    renderPosition = relativePosition(-5.94f, -6.95f, 31.68f, 21.78f);
}

void Player::prepare() {
    body = new Animation(scene->sprites[Klad1::SPRITE_PLAYER], IDLE);
    step = scene->sounds[Klad1::SOUND_STEP];
}

void Player::scan() {
    inLadder = false;
    aboveLadder = false;
    nearBridge = 0;
    foundBridge = false;
    wrongBridge = false;

    if (world == nullptr || !isReady()) {
        return;
    }

    float top = getY();
    float feet = getY() + getHeight();    
    float left = getX();
    float right = getX()+getWidth();
    ladderCount = 0;
    
    scanUnits(2, 2, 2, 2, [&](Unit* unit) {
        switch (unit->getType()) {
            case UNIT_LADDER:
                if (intersects(unit)) {
                    bool sameLadderRow =
                        feet > unit->getY() &&
                        feet < unit->getY() + unit->getHeight();

                    // Horizontal ladder intersection count
                    if (sameLadderRow
                    ) {
                        ladderCount++;
                    }
                    if (
                        (feet > unit->getY() && feet<unit->getY()+unit->getHeight()+LADDER_FALL_HOLD
                        && left>=unit->getX()-LADDER_FALL_HOLD && right<=unit->getX()+unit->getWidth()+LADDER_FALL_HOLD)
                        || ladderCount==2
                        // || (ladderCount==1 && feet>=unit->getY()+unit->getHeight())
                    ) {
                        inLadder = true;
                    }
                } else if (intersects(unit,0,0,0,2)) {
                    aboveLadder = true;
                }
                break;

            case UNIT_BRIDGE:
                if (!intersects(unit) && (intersects(unit, 2, 0, 0, 0) || intersects(unit, 0, 0, 2, 0))) {
                    foundBridge = true;
                    nearBridge =
                        (unit->getY() + unit->getHeight()) -
                        (getY() + getHeight());
                }
                 else if (intersects(unit)) {
                    wrongBridge = true;
                }
                break;
        }
        return true;
    });

}

void Player::update(Context* context) {
    scan();

    Keyboard* key = context->input->keyboard;
    if (key->a || key->left) {
        facingRight = false;
    } else if (key->d || key->right) {
        facingRight = true;
    }

    float fallStep = (static_cast<float>(context->clock->delta) * speed) / 100.0f;
    bool fallingNow = !inLadder && !aboveLadder && canMove(0, fallStep);
    bool showShoot = shootCooldown.isActive() || key->space;
    int previousClip = body->activeClip;
    int previousFrame = body->frame;
    bool playedStep = false;

    if (fallingNow) {
        float deltaX = 0;
        float deltaY = 0;
        debugMoveX = 0;
        debugMoveY = 1;
        move(context->clock->delta, 0, 1, deltaX, deltaY);
        debugDeltaX = deltaX;
        debugDeltaY = deltaY;
        falling += deltaY;
        wasFalling = true;
        if (showShoot) {
            body->play(facingRight ? SHOOT_RIGHT : SHOOT_LEFT);
        } else {
            body->play(IDLE);
        }
        body->update(context->clock->delta);
        return;
    }

    falling = 0;
    if (wasFalling) {
        wasFalling = false;
        if (step != nullptr) {
            step->play(context);
            playedStep = true;
        }
    }

    float moveX = 0;
    float moveY = 0;
    bool movedNow = false;
    float deltaX = 0;
    float deltaY = 0;

    if (!showShoot) {
        if (inLadder) {
            if (key->w || key->up) {
                moveY = -1;
            } 
        }

        if (key->s || key->down) {
            moveY = 1;
        }

        if (key->a || key->left) {
            moveX = -1;
            facingRight = false;
            if (foundBridge && nearBridge <= 1 && nearBridge > BRIDGE_SNAP_UP) {
                moveY = -1;
            }
        } else if (key->d || key->right) {
            moveX = 1;
            facingRight = true;
            if (foundBridge && nearBridge <= 1 && nearBridge > BRIDGE_SNAP_UP) {
                moveY = -1;
            }
        }
    }

    if (!showShoot && (moveX != 0 || moveY != 0)) {
        movedNow = move(context->clock->delta, moveX, moveY, deltaX, deltaY);
    }

    debugMoveX = moveX;
    debugMoveY = moveY;
    debugDeltaX = deltaX;
    debugDeltaY = deltaY;

    if (showShoot) {
        body->play(facingRight ? SHOOT_RIGHT : SHOOT_LEFT);
    } else if (!movedNow) {
        body->play(IDLE);
    } else if (deltaX == 0 && deltaY != 0 && inLadder) {
        body->play(CLIMB, 1.15f);
    } else if (deltaX < 0) {
        body->play(RUN_LEFT, 1.15f);
    } else if (deltaX > 0) {
        body->play(RUN_RIGHT, 1.15f);
    } else {
        body->play(IDLE);
    }

    body->update(context->clock->delta);

    if (!playedStep && body->activeClip == previousClip && previousFrame != body->frame) {
        if ((body->activeClip == RUN_LEFT || body->activeClip == RUN_RIGHT) &&
            (body->frame == 1 || body->frame == 3)) {
            if (step != nullptr) {
                step->play(context);
            }
        } else if (body->activeClip == CLIMB && body->frame == 0) {
            if (step != nullptr) {
                step->play(context);
            }
        }
    }

}

void Player::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition())) {
        // print(getPosition(), 
        //     "inLadder", inLadder,
        //     "aboveLadder", aboveLadder,
        //     "ladderCount", ladderCount,
        //     "x", getX(),
        //     "y", getY()
        // );
        body->render(context->camera->translate(getRenderPosition()));
    }
}

bool Player::canCrossUnit(Unit* target) const {
    switch (target->getType())
    {
        case UNIT_LADDER:
            return true;

        case UNIT_BRIDGE:
            if (falling>BRIDGE_FALL_HOLD || nearBridge<=BRIDGE_SNAP_UP || wrongBridge)
                return true;
            break;

        case UNIT_BRICK:
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
        case UNIT_ENEMY:
            return true;
    }

    return Unit::canCrossUnit(target);
}

int Player::getType() const {
    return UNIT_PLAYER;
}

void Player::respawn(float x, float y) {
    falling = 0;
    inLadder = false;
    aboveLadder = false;
    nearBridge = 0;
    foundBridge = false;
    wrongBridge = false;
    setPosition(x, y);
    body->play(IDLE);
}

bool Player::isFalling() const {
    return falling > 0;
}

void Player::shoot() {
    shootCooldown.reset();
    body->play(facingRight ? SHOOT_RIGHT : SHOOT_LEFT);
}

bool Player::isFacingRight() const {
    return facingRight;
}

float Player::getFalling() const {
    return falling;
}

void Player::restore(float x, float y, bool facingRight, float falling, bool wasFalling) {
    setPosition(x, y);
    this->facingRight = facingRight;
    this->falling = falling;
    this->wasFalling = wasFalling;
}

float Player::getBulletX() const {
    SDL_FRect* render = renderPosition->getPosition();
    return facingRight ? render->x + render->w - 10.0f : render->x + 2.0f;
}

float Player::getBulletY() const {
    SDL_FRect* render = renderPosition->getPosition();
    return render->y + 10.0f;
}

Player::~Player() {
    delete body;
}
