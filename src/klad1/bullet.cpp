#include "klad1/bullet.h"
#include "klad1/klad1.h"
#include "klad1/type.h"
#include "klad1/unit_type.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/clock.h"
#include "game/context.h"
#include "game/sprite.h"

Bullet::Bullet(Sprite* sprite, float x, float y, bool facingRight) {
    setLayer(Klad1::LAYER_WORLD);
    setSize(8, 4);
    setPosition(x, y);
    direction = facingRight ? 1.0f : -1.0f;
    speed = 18.0f;
    body = new Animation(sprite, facingRight ? 1 : 2);
    renderPosition = relativePosition(-12, -9, 32, 22);
}

void Bullet::update(Context* context) {
    float deltaX = direction * static_cast<float>(context->clock->delta) * (speed / 100.0f);
    addPosition(deltaX, 0);
    body->update(context->clock->delta);

    if (world == nullptr) {
        return;
    }

    if (getX() + getWidth() < 0 ||
        getX() > world->getWidth() ||
        getY() + getHeight() < 0 ||
        getY() > world->getHeight()) {
        dead = true;
        return;
    }

    if (touchesTile(TILE_WALL)) {
        dead = true;
        return;
    }

    scanUnits(0, 0, 0, 0, [&](Unit* unit) {
        if (unit->getType() == UNIT_BRICK) {
            dead = true;
            return false;
        }
        return true;
    });
}

void Bullet::render(Context* context) {
    if (!dead && context->camera->isVisible(getRenderPosition())) {
        body->render(context->camera->translate(getRenderPosition()));
    }
}

bool Bullet::isDead() const {
    return dead;
}

int Bullet::getType() const {
    return UNIT_BULLET;
}

Bullet::~Bullet() {
    delete body;
}
