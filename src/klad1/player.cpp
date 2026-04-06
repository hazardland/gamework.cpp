#include "klad1/bridge.h"
#include "klad1/kind.h"
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

namespace {
Bridge* findBridge(Player* player, float probeLeft, float probeRight, float probeTop, float probeBottom) {
    Map* map = player->map;
    if (map == nullptr) {
        return nullptr;
    }

    int fromX = std::clamp(static_cast<int>(probeLeft / map->cellWidth), 0, map->gridWidth - 1);
    int toX = std::clamp(static_cast<int>((probeRight - 0.001f) / map->cellWidth), 0, map->gridWidth - 1);
    int fromY = std::clamp(static_cast<int>(probeTop / map->cellHeight), 0, map->gridHeight - 1);
    int toY = std::clamp(static_cast<int>((probeBottom - 0.001f) / map->cellHeight), 0, map->gridHeight - 1);

    for (int x = fromX; x <= toX; x++) {
        for (int y = fromY; y <= toY; y++) {
            for (Unit* unit : map->grid[x][y]->units[0]) {
                Bridge* bridge = dynamic_cast<Bridge*>(unit);
                if (bridge == nullptr) {
                    continue;
                }

                if (probeLeft < bridge->getX() + bridge->getWidth() &&
                    probeRight > bridge->getX() &&
                    probeTop < bridge->getY() + bridge->getHeight() &&
                    probeBottom > bridge->getY()) {
                    return bridge;
                }
            }
        }
    }

    return nullptr;
}
}

Player::Player(Sprite* sprite, TTF_Font* font) {
    setSize(19.8f, 15.84f);
    setLayer(1);
    allowTerrain(TERRAIN_BLANK);
    speed = 11.25f;
    body = new Animation(sprite, IDLE);
    renderPosition = createChildPosition(-5.94f, -4.95f, 31.68f, 21.78f);
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

    constexpr float supportMargin = 2.0f;
    constexpr float supportTolerance = 2.0f;

    float playerLeft = getX();
    float playerRight = getX() + getWidth();
    float feetY = getY() + getHeight();
    float probeLeft = playerLeft + supportMargin;
    float probeRight = playerRight - supportMargin;

    if (probeRight <= probeLeft) {
        return false;
    }

    Bridge* bridge = findBridge(
        this,
        probeLeft,
        probeRight,
        feetY,
        feetY + supportTolerance
    );

    if (bridge == nullptr) {
        return false;
    }

    return playerLeft >= bridge->getX() && playerRight <= bridge->getX() + bridge->getWidth();
}

bool Player::isHalfBridge() {
    if (map == nullptr || !position->isReady()) {
        return false;
    }

    constexpr float supportMargin = 2.0f;
    constexpr float supportTolerance = 2.0f;

    float playerLeft = getX();
    float playerRight = getX() + getWidth();
    float feetY = getY() + getHeight();
    float probeLeft = playerLeft + supportMargin;
    float probeRight = playerRight - supportMargin;

    if (probeRight <= probeLeft) {
        return false;
    }

    Bridge* bridge = findBridge(
        this,
        probeLeft,
        probeRight,
        feetY,
        feetY + supportTolerance
    );

    if (bridge == nullptr) {
        return false;
    }

    return !(playerLeft >= bridge->getX() && playerRight <= bridge->getX() + bridge->getWidth());
}

void Player::update(State* state) {
    bool inLadder = insideLadder();
    bool aboveLadder = touchesTerrain(TERRAIN_LADDER, 4, 0, -4, 1) && !inLadder;
    bool isInsideBridge = insideBridge();
    if (!insideBridge()) {
        bridgeWalkable = isHalfBridge() && falling < 15;
    }
    // bool aboveBridge = touchesTerrain(TERRAIN_BRIDGE, 0, 0, 0, 1);
    debug->setText(
        "L:" + std::to_string(inLadder) +
        " A:" + std::to_string(aboveLadder) +
        " BW:" + std::to_string(bridgeWalkable) +
        " IB:" + std::to_string(isInsideBridge) +
        " F:" + std::to_string(static_cast<int>(falling))
    );

    if (!inLadder && !aboveLadder && !bridgeWalkable && canMove(0, 1)) {
        bridgeWalkable = false;
        float deltaX = 0;
        float deltaY = 0;
        move(state->clock->delta, 0, 1, deltaX, deltaY);
        falling += deltaY;
        body->play(IDLE);
        body->update(state->clock->delta);
        return;
    }

    falling = 0;

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

bool Player::shouldCollide(Unit* target) const {
    if (falling > 0 && target->getKind() == KIND_BRIDGE) {
        return false;
    }

    return Unit::shouldCollide(target);
}

Player::~Player() {
    delete body;
    delete debug;
}
