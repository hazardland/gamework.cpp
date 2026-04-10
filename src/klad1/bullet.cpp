#include "klad1/bullet.h"
#include "klad1/brick.h"
#include "klad1/klad1.h"
#include "klad1/type.h"
#include "klad1/unit_type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/cell.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/scene.h"
#include "game/sprite.h"
#include "game/debug.h"
#include "game/sound.h"

#include <algorithm>

Bullet::Bullet(float x, float y, bool facingRight) {
    setLayer(Klad1::LAYER_WORLD);
    setSize(8, 4);
    setPosition(x, y);
    direction = facingRight ? 1.0f : -1.0f;
    speed = 18.0f;
    renderPosition = relativePosition(-12, -9, 32, 22);
}

void Bullet::prepare() {
    body = new Animation(scene->sprites[Klad1::SPRITE_BULLET], direction > 0 ? 1 : 2);
    hit = scene->sounds[Klad1::SOUND_HIT];
}

float Bullet::findWallHitX(float startFront, float endFront) {
    if (world == nullptr) {
        return -1.0f;
    }

    int fromY = std::clamp(static_cast<int>(getY() / world->cellHeight), 0, world->gridHeight - 1);
    int toY = std::clamp(static_cast<int>((getY() + getHeight()) / world->cellHeight), 0, world->gridHeight - 1);

    if (direction > 0) {
        int fromX = std::clamp(static_cast<int>(startFront / world->cellWidth), 0, world->gridWidth - 1);
        int toX = std::clamp(static_cast<int>(endFront / world->cellWidth), 0, world->gridWidth - 1);

        for (int cellX = fromX; cellX <= toX; cellX++) {
            for (int cellY = fromY; cellY <= toY; cellY++) {
                if (world->grid[cellX][cellY]->type == TILE_WALL) {
                    return cellX * world->cellWidth;
                }
            }
        }
    } else {
        int fromX = std::clamp(static_cast<int>(endFront / world->cellWidth), 0, world->gridWidth - 1);
        int toX = std::clamp(static_cast<int>(startFront / world->cellWidth), 0, world->gridWidth - 1);

        for (int cellX = toX; cellX >= fromX; cellX--) {
            for (int cellY = fromY; cellY <= toY; cellY++) {
                if (world->grid[cellX][cellY]->type == TILE_WALL) {
                    return (cellX + 1) * world->cellWidth;
                }
            }
        }
    }

    return -1.0f;
}

void Bullet::update(Context* context) {
    if (dying) {
        dead = true;
        return;
    }

    if (world == nullptr) {
        return;
    }

    Brick::HitSide hitSide = direction > 0 ? Brick::HitSide::Left : Brick::HitSide::Right;

    if (touchesTile(TILE_WALL)) {
        hit->play(context);
        dying = true;
        return;
    }

    Brick* overlappingBrick = nullptr;
    scanUnits(0, 0, 0, 0, [&](Unit* unit) {
        if (unit->getType() != UNIT_BRICK) {
            return true;
        }

        Brick* brick = static_cast<Brick*>(unit);
        if (!brick->isSolid()) {
            return true;
        }

        overlappingBrick = brick;
        return false;
    });

    if (overlappingBrick != nullptr) {
        float hitX = overlappingBrick->getHitX(hitSide);
        float frontX = direction > 0 ? getX() + getWidth() : getX();

        if ((direction > 0 && frontX >= hitX) ||
            (direction < 0 && frontX <= hitX)) {
            overlappingBrick->hit(hitSide);
            hit->play(context);
            dying = true;
            return;
        }
    }

    float deltaX = direction * static_cast<float>(context->clock->delta) * (speed / 100.0f);
    body->update(context->clock->delta);

    float startFront = direction > 0 ? getX() + getWidth() : getX();
    float endFront = startFront + deltaX;
    float wallHitX = findWallHitX(startFront, endFront);
    float brickHitX = -1.0f;
    Brick* hitBrick = nullptr;

    scanUnits(std::max(deltaX, 0.0f), 0, std::max(-deltaX, 0.0f), 0, [&](Unit* unit) {
        if (unit->getType() != UNIT_BRICK) {
            return true;
        }

        Brick* brick = static_cast<Brick*>(unit);
        float hitX = brick->getHitX(hitSide);
        if (hitX < 0) {
            return true;
        }

        if (direction > 0) {
            if (hitX < startFront || hitX > endFront) {
                return true;
            }
            if (hitBrick == nullptr || hitX < brickHitX) {
                hitBrick = brick;
                brickHitX = hitX;
            }
            return true;
        }

        if (hitX > startFront || hitX < endFront) {
            return true;
        }
        if (hitBrick == nullptr || hitX > brickHitX) {
            hitBrick = brick;
            brickHitX = hitX;
        }
        return true;
    });

    bool hitWall = wallHitX >= 0;
    bool hitBrickNow = hitBrick != nullptr;

    if (hitWall && hitBrickNow) {
        if ((direction > 0 && wallHitX <= brickHitX) ||
            (direction < 0 && wallHitX >= brickHitX)) {
            hitBrickNow = false;
        } else {
            hitWall = false;
        }
    }

    if (hitWall) {
        setX(direction > 0 ? wallHitX - getWidth() : wallHitX);
        hit->play(context);
        dying = true;
        return;
    }

    if (hitBrickNow) {
        setX(direction > 0 ? brickHitX - getWidth() : brickHitX);
        hitBrick->hit(hitSide);
        hit->play(context);
        dying = true;
        return;
    }

    addPosition(deltaX, 0);

    if (getX() + getWidth() < 0 ||
        getX() > world->getWidth() ||
        getY() + getHeight() < 0 ||
        getY() > world->getHeight()) {
        dying = true;
    }
}

void Bullet::render(Context* context) {
    if (!dead && context->camera->isVisible(getRenderPosition())) {
        // draw(getPosition());
        body->render(context->camera->translate(getRenderPosition()));
    }
}

bool Bullet::isDead() const {
    return dead;
}

bool Bullet::isFacingRight() const {
    return direction > 0;
}

int Bullet::getType() const {
    return UNIT_BULLET;
}

Bullet::~Bullet() {
    delete body;
}
