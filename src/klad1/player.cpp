#include "klad1/player.h"
#include "klad1/terrain.h"

#include "game/animation.h"
#include "game/camera.h"
#include "game/cell.h"
#include "game/clock.h"
#include "game/input.h"
#include "game/map.h"
#include "game/position.h"
#include "game/sprite.h"
#include "game/state.h"
#include "game/terrain.h"
#include "game/text.h"

#include <cmath>

Player::Player(Sprite* sprite, TTF_Font* font) {
    setSize(22.0f, 17.6f);
    setLayer(1);
    allowTerrain(TERRAIN_BLANK);
    allowTerrain(TERRAIN_BRIDGE);
    speed = 11.25f;
    body = new Animation(sprite, IDLE);
    renderPosition = createChildPosition(-6.6f, -5.5f, 35.2f, 24.2f);
    debug = new Text(font, createChildPosition(-6, -18));
}

bool Player::insideLadder() {
    if (map == nullptr || !position->isReady()) {
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

bool Player::insideBridge() {
    if (map == nullptr || !position->isReady()) {
        return false;
    }

    float playerLeft = getX();
    float playerRight = getX() + getWidth();

    int fromX = std::clamp(static_cast<int>(getX() / map->cellWidth), 0, map->gridWidth - 1);
    int fromY = std::clamp(static_cast<int>(getY() / map->cellHeight), 0, map->gridHeight - 1);
    int toX = std::clamp(static_cast<int>((getX() + getWidth()) / map->cellWidth), 0, map->gridWidth - 1);
    int toY = std::clamp(static_cast<int>((getY()+1 + getHeight()) / map->cellHeight), 0, map->gridHeight - 1);

    for (int x = fromX; x <= toX; x++) {
        for (int y = fromY; y <= toY; y++) {
            Terrain* terrain = map->grid[x][y]->terrain;
            if (terrain != nullptr && terrain->id == TERRAIN_BRIDGE) {
                float bridgeLeft = x * map->cellWidth;
                float bridgeRight = x * map->cellWidth + map->cellWidth;
                return playerLeft >= bridgeLeft && playerRight <= bridgeRight;
            }
        }
    }

    return false;
}

bool Player::isHalfBridge() {
    return touchesTerrain(TERRAIN_BRIDGE, 0, 0, 0, 1);
}

void Player::update(State* state) {
    bool inLadder = insideLadder();
    bool aboveLadder = touchesTerrain(TERRAIN_LADDER, 2, 0, -2, 1) && !inLadder;
    bool isInsideBridge = insideBridge();
    if (!insideBridge()) {
        bridgeWalkable = isHalfBridge();
    }
    // bool aboveBridge = touchesTerrain(TERRAIN_BRIDGE, 0, 0, 0, 1);
    debug->setText(
        "L:" + std::to_string(inLadder) +
        " A:" + std::to_string(aboveLadder) +
        " BW:" + std::to_string(bridgeWalkable) +
        " IB:" + std::to_string(isInsideBridge)
    );

    if (!inLadder && !aboveLadder && !bridgeWalkable && canMove(0, 1)) {
        bridgeWalkable = false;
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
