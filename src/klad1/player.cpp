#include "klad1/player.h"
#include "klad1/level.h"
#include "klad1/terrain.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/cell.h"
#include "game/clock.h"
#include "game/input.h"
#include "game/map.h"
#include "game/sprite.h"
#include "game/state.h"
#include "game/terrain.h"
#include "game/text.h"

#include <cmath>

Player::Player(Sprite* sprite, TTF_Font* font) {
    setSize(22.0f, 17.6f);
    setLayer(1);
    allowTerrain(TERRAIN_BLANK);
    speed = 11.25f;
    body = new Animation(sprite, IDLE);
    renderPosition = createChildPosition(-6.6f, -5.5f, 35.2f, 24.2f);
    debug = new Text(font, createChildPosition(-6, -18));
}

bool Player::insideLadder() {
    if (map == nullptr) {
        return false;
    }

    float playerLeft = getX();
    float playerRight = getX() + getWidth();

    int fromX = std::clamp(static_cast<int>(getX() / map->cellWidth), 0, map->gridWidth - 1);
    int fromY = std::clamp(static_cast<int>(getY() / map->cellHeight), 0, map->gridHeight - 1);
    int toX = std::clamp(static_cast<int>((getX() + getWidth()) / map->cellWidth), 0, map->gridWidth - 1);
    int toY = std::clamp(static_cast<int>((getY() + getHeight()) / map->cellHeight), 0, map->gridHeight - 1);

    for (int x = fromX; x <= toX; x++) {
        for (int y = fromY; y <= toY; y++) {
            Terrain* terrain = map->grid[x][y]->terrain;
            if (terrain != nullptr && terrain->id == TERRAIN_LADDER) {
                float ladderLeft = x * map->cellWidth + 0;
                float ladderRight = x * map->cellWidth + map->cellWidth - 0;
                return playerLeft >= ladderLeft && playerRight <= ladderRight;
            }
        }
    }

    return false;
}

bool Player::isOnBridgeTop() {
    if (map == nullptr) {
        return false;
    }

    constexpr float supportMargin = 2.0f;
    constexpr float supportTolerance = 2.0f;

    float feetY = getY() + getHeight();
    float probeLeft = getX() + supportMargin;
    float probeRight = getX() + getWidth() - supportMargin;

    if (probeRight <= probeLeft) {
        return false;
    }

    int fromX = std::clamp(static_cast<int>(probeLeft / map->cellWidth), 0, map->gridWidth - 1);
    int toX = std::clamp(static_cast<int>((probeRight - 0.001f) / map->cellWidth), 0, map->gridWidth - 1);
    int fromY = std::clamp(static_cast<int>(feetY / map->cellHeight), 0, map->gridHeight - 1);
    int toY = std::clamp(static_cast<int>((feetY + supportTolerance) / map->cellHeight), 0, map->gridHeight - 1);

    for (int x = fromX; x <= toX; x++) {
        for (int y = fromY; y <= toY; y++) {
            float bridgeTop = y * map->cellHeight;
            if (std::fabs(feetY - bridgeTop) > supportTolerance) {
                continue;
            }

            if (map->grid[x][y]->tile == Level::BRIDGE) {
                return true;
            }
        }
    }

    return false;
}

bool Player::isAboveBridge() {
    if (map == nullptr) {
        return false;
    }

    constexpr float supportMargin = 2.0f;
    constexpr float supportTolerance = 2.0f;

    float feetY = getY() + getHeight();
    float probeLeft = getX() + supportMargin;
    float probeRight = getX() + getWidth() - supportMargin;

    if (probeRight <= probeLeft) {
        return false;
    }

    int fromX = std::clamp(static_cast<int>(probeLeft / map->cellWidth), 0, map->gridWidth - 1);
    int toX = std::clamp(static_cast<int>((probeRight - 0.001f) / map->cellWidth), 0, map->gridWidth - 1);
    int fromY = std::clamp(static_cast<int>((feetY + 1) / map->cellHeight), 0, map->gridHeight - 1);
    int toY = std::clamp(static_cast<int>((feetY + supportTolerance + 1) / map->cellHeight), 0, map->gridHeight - 1);

    for (int x = fromX; x <= toX; x++) {
        for (int y = fromY; y <= toY; y++) {
            float bridgeTop = y * map->cellHeight;
            if (bridgeTop <= feetY || bridgeTop - feetY > supportTolerance + 1) {
                continue;
            }

            if (map->grid[x][y]->tile == Level::BRIDGE) {
                return true;
            }
        }
    }

    return false;
}

void Player::update(State* state) {
    bool inLadder = insideLadder();
    bool aboveLadder = touchesTerrain(TERRAIN_LADDER, 2, 0, -2, 1) && !inLadder;
    bool onBridgeTop = isOnBridgeTop();
    bool aboveBridge = isAboveBridge();
    debug->setText(
        "L:" + std::to_string(inLadder) +
        " A:" + std::to_string(aboveLadder) +
        " B:" + std::to_string(onBridgeTop) +
        " AB:" + std::to_string(aboveBridge)
    );

    if (!inLadder && !aboveLadder && !onBridgeTop && canMove(0, 1)) {
        float deltaX = 0;
        float deltaY = 0;
        move(state->clock->delta, 0, 1, deltaX, deltaY);
        body->play(IDLE);
        body->update(state->clock->delta);
        return;
    }

    Keyboard* key = state->input->keyboard;

    float moveX = 0;
    float moveY = 0;

    if (inLadder && !aboveLadder) {
        if (key->w || key->up) {
            moveY = -1;
        } 
    }

    if (key->s || key->down) {
        moveY = 1;
    }

    if (key->a || key->left) {
        moveX = -1;
    } else if (key->d || key->right) {
        moveX = 1;
    }

    if (moveX != 0 || moveY != 0) {
        float deltaX = 0;
        float deltaY = 0;
        move(state->clock->delta, moveX, moveY, deltaX, deltaY);
    }

    if (moveY != 0 && inLadder) {
        body->play(CLIMB);
    } else if (moveX < 0) {
        body->play(RUN_LEFT);
    } else if (moveX > 0) {
        body->play(RUN_RIGHT);
    } else {
        body->play(IDLE);
    }

    body->update(state->clock->delta);
}

void Player::render(State* state) {
    if (state->camera->isVisible(getRenderPosition())) {
        drawPosition(state);
        body->render(state->camera->translate(getRenderPosition()));
        debug->render(state);
    }
}

Player::~Player() {
    delete body;
    delete debug;
}
