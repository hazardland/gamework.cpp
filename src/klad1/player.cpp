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
    nearLadder = 0;
    foundBridge = false;
    wrongBridge = false;

    if (world == nullptr || !isReady()) {
        return;
    }

    float feetTop = getY() + getHeight() - 1;
    float feetBottom = getY() + getHeight();    
    
    scanUnits(1, 1, 1, 1, [&](Unit* unit) {
        switch (unit->getType()) {
            case UNIT_LADDER:
                if (intersects(unit)) {
                    nearLadder = unit->getY() + unit->getHeight() - (getY() + getHeight()*2);
                    if (getX() < unit->getX() + unit->getWidth() &&
                        getX() + getWidth() > unit->getX() &&
                        feetTop < unit->getY() + unit->getHeight() &&
                        feetBottom > unit->getY()) {

                        inLadder = true;
                    }           
                } else if (intersects(unit,0,0,0,1)) {
                    aboveLadder = true;
                }
                break;

            case UNIT_BRIDGE:
                if (!intersects(unit) && (intersects(unit, 1, 0, 0, 0) || intersects(unit, 0, 0, 1, 0))) {
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
    bool fallingNow = !inLadder && !aboveLadder && canMove(0, 1);
    bool showShoot = shootCooldown.isActive() || (key->space && !fallingNow);
    int previousClip = body->activeClip;
    int previousFrame = body->frame;
    bool playedStep = false;

    if (fallingNow) {
        float deltaX = 0;
        float deltaY = 0;
        move(context->clock->delta, 0, 1, deltaX, deltaY);
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

    if (!showShoot) {
        if (inLadder) { // && !aboveLadder
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
            if (nearBridge!=0 && nearBridge<1 && nearBridge>-5){
                moveY = -1;
            }
        } else if (key->d || key->right) {
            moveX = 1;
            facingRight = true;
            if (nearBridge!=0 && nearBridge<1 && nearBridge>-5){
                moveY = -1;
            }
        }
    }

    if (!showShoot && (moveX != 0 || moveY != 0)) {
        float deltaX = 0;
        float deltaY = 0;
        move(context->clock->delta, moveX, moveY, deltaX, deltaY);
    }

    if (showShoot) {
        body->play(facingRight ? SHOOT_RIGHT : SHOOT_LEFT);
    } else if (moveY != 0 && inLadder) {
        body->play(CLIMB);
    } else if (moveX < 0) {
        body->play(RUN_LEFT);
    } else if (moveX > 0) {
        body->play(RUN_RIGHT);
    } else {
        body->play(IDLE);
    }

    body->update(context->clock->delta);

    if (!playedStep &&
        (body->activeClip == RUN_LEFT || body->activeClip == RUN_RIGHT || body->activeClip == CLIMB) &&
        body->activeClip == previousClip &&
        previousFrame != body->frame &&
        body->frame == 0) {
        if (step != nullptr) {
            step->play(context);
        }
    }
}

void Player::render(Context* context) {
    if (context->camera->isVisible(getRenderPosition())) {
        // draw(getPosition());
        // print(
            // getPosition(),
            // "IL", inLadder,
            // "AL", aboveLadder,
            // "FB", foundBridge,
            // "NB", nearBridge
            // "NL", nearLadder
            // "F", static_cast<int>(falling)
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
            if (falling>3 || nearBridge<=-5 || wrongBridge)
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
            return true;
    }

    return Unit::canCrossUnit(target);
}

void Player::respawn(float x, float y) {
    falling = 0;
    inLadder = false;
    aboveLadder = false;
    nearBridge = 0;
    nearLadder = 0;
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
