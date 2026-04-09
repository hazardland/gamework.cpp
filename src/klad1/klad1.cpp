#include "klad1/klad1.h"
#include "klad1/bridge.h"
#include "klad1/brick.h"
#include "klad1/door.h"
#include "klad1/gate.h"
#include "klad1/gold.h"
#include "klad1/key.h"
#include "klad1/ladder.h"
#include "klad1/level.h"
#include "klad1/levels/level1.h"
#include "klad1/player.h"
#include "klad1/bullet.h"
#include "klad1/tide.h"

#include "game/camera.h"
#include "game/clip.h"
#include "game/frame.h"
#include "game/image.h"
#include "game/input.h"
#include "game/world.h"
#include "game/sprite.h"
#include "game/context.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <random>

void Klad1::prepare(Context* context) {
    Scene::prepare(context);
    doors.clear();
    gates.clear();
    golds.clear();
    bullets.clear();
    tides.clear();
    key = nullptr;
    lives = 5;
    hasKey = false;
    goldCollected = 0;
    keyGold = -1;

    image = new Image(renderer, "assets/klad1/sprites.png");
    font = TTF_OpenFont("assets/war2/fonts/titillium.ttf", 12);
    TTF_SetFontOutline(font, 1);
    setDebugFont(font);

    world = new World(
        image,
        CELL_WIDTH,
        CELL_HEIGHT,
        Level::WIDTH,
        Level::HEIGHT,
        2
    );

    sprites[SPRITE_BRICK] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))
        ->addClip(1, 8, 0, 1, false, false)
        ->addClip(2, 12, 0, 1, false, false)
        ->addClip(3, 13, 0, 1, false, false)
        ->addClip(4, 14, 0, 1, false, false)
        ->addClip(5, 15, 0, 1, false, false)
        ->addClip(6, 16, 0, 1, false, false)
        ->addClip(7, 17, 0, 1, false, false)
        ->addClip(8, 18, 0, 1, false, false)
        ->addClip(9, 19, 0, 1, false, false)
        ->addClip(10, 20, 0, 1, false, false)
        ->addClip(11, 46, 0, 1, false, false);
    sprites[SPRITE_BRIDGE] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 7, 0, 1, false, false);
    sprites[SPRITE_LADDER] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 6, 0, 1, false, false);
    sprites[SPRITE_GOLD] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 1, 0, 1, false, false);
    sprites[SPRITE_DOOR] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))
        ->addClip(1, 2, 0, 1, false, false)
        ->addClip(2, 11, 0, 1, false, false);
    sprites[SPRITE_TIDE] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 4, 0, 1, false, false);
    sprites[SPRITE_KEY] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 10, 0, 1, false, false);
    sprites[SPRITE_BULLET] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))
        ->addClip(1, 31, 0, 1, false, false)
        ->addClip(2, 31, 0, 1, true, false);
    sprites[SPRITE_PLAYER] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT, 133))
        ->addClip(1, 24, 0, 1, false, false)
        ->addClip(2, 25, 0, 2, false, false)
        ->addClip(3, 26, 0, 4, false, false)
        ->addClip(4, 26, 0, 4, true, false)
        ->addClip(5, 26, 0, 1, true, false)
        ->addClip(6, 26, 0, 1, false, false);

    Clip* climbClip = sprites[SPRITE_PLAYER]->getClip(2);
    climbClip->frames[1].rect = climbClip->frames[0].rect;
    climbClip->frames[1].flipX = true;
    climbClip->frames[1].flipY = false;
    climbClip->frames[1].cacheFlip();

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> keyGoldDist(0, static_cast<int>(level1.keyGolds.size()) - 1);
    keyGold = level1.keyGolds[keyGoldDist(rng)];

    std::vector<Position> tideSpawns;

    for (int y = 0; y < Level::HEIGHT; y++) {
        for (int x = 0; x < Level::WIDTH; x++) {
            uint8_t value = level1.grid[y][x];
            world->setCell(x, y, TILE_BLANK, Level::BLANK);

            switch (value) {
                case Level::GOLD: {
                    Gold* gold = new Gold(
                        sprites[SPRITE_GOLD],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT,
                        static_cast<int>(golds.size())
                    );
                    addObject(gold);
                    golds.push_back(gold);
                    break;
                }

                case Level::DOOR: {
                    Door* door = new Door(
                        sprites[SPRITE_DOOR],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    door->setWorld(world);
                    addObject(door);
                    doors.push_back(door);
                    break;
                }

                case Level::EXIT: {
                    Gate* gate = new Gate(
                        sprites[SPRITE_DOOR],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    gate->setWorld(world);
                    addObject(gate);
                    gates.push_back(gate);
                    break;
                }

                case Level::TIDE: {
                    tideSpawns.push_back({
                        static_cast<uint8_t>(x),
                        static_cast<uint8_t>(y)
                    });
                    break;
                }

                case Level::WATER:
                    world->setCell(x, y, TILE_WATER, Level::WATER);
                    break;

                case Level::BRICK: {
                    Brick* brick = new Brick(
                        sprites[SPRITE_BRICK],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    brick->setWorld(world);
                    addObject(brick);
                    break;
                }

                case Level::LADDER: {
                    Ladder* ladder = new Ladder(
                        sprites[SPRITE_LADDER],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    ladder->setWorld(world);
                    addObject(ladder);
                    break;
                }

                case Level::BRIDGE:
                {
                    Bridge* bridge = new Bridge(
                        sprites[SPRITE_BRIDGE],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    bridge->setWorld(world);
                    addObject(bridge);
                    break;
                }

                case Level::WALL:
                    world->setCell(x, y, TILE_WALL, Level::WALL);
                    break;

                default:
                    break;
            }
        }
    }

    player = new Player(
        sprites[SPRITE_PLAYER],
        level1.playerSpawn.x * CELL_WIDTH,
        level1.playerSpawn.y * CELL_HEIGHT
    );
    player->setWorld(world);
    addObject(player);
    playerSpawnX = player->getX();
    playerSpawnY = player->getY();

    for (const Position& tidePosition : tideSpawns) {
        Tide* tide = new Tide(
            sprites[SPRITE_TIDE],
            tidePosition.x * CELL_WIDTH,
            tidePosition.y * CELL_HEIGHT
        );
        addObject(tide);
        this->tides.push_back(tide);
    }

    addFps(font);

    context->camera->setZoom(1.5f);
}

void Klad1::update(Context* context) {
    Scene::update(context);

    if (player == nullptr) {
        return;
    }

    Keyboard* keyboard = context->input->keyboard;
    if (!player->isFalling() && keyboard->space && bulletCooldown.isReady()) {
        Bullet* bullet = new Bullet(
            sprites[SPRITE_BULLET],
            player->getBulletX(),
            player->getBulletY(),
            player->isFacingRight()
        );
        bullet->setWorld(world);
        addObject(bullet);
        bullets.push_back(bullet);
        bulletCooldown.reset();
        player->shoot();
    }

    for (Tide* tide : tides) {
        if (tide != nullptr && player->intersects(tide)) {
            if (lives > 0) {
                lives--;
            }
            player->respawn(playerSpawnX, playerSpawnY);
            break;
        }
    }

    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet* bullet = *it;
        if (bullet == nullptr || !bullet->isDead()) {
            ++it;
            continue;
        }

        removeObject(bullet);
        delete bullet;
        it = bullets.erase(it);
    }

    bool spawnedKeyThisFrame = false;

    for (Gold* gold : golds) {
        if (gold == nullptr || gold->isCollected()) {
            continue;
        }

        if (!player->intersects(gold)) {
            continue;
        }

        gold->collect();
        goldCollected++;

        if (gold->getIndex() == keyGold && key == nullptr) {
            key = new Key(sprites[SPRITE_KEY], gold->getX(), gold->getY());
            addObject(key);
            spawnedKeyThisFrame = true;
        }
    }

    if (!spawnedKeyThisFrame && key != nullptr && !hasKey && player->intersects(key)) {
        hasKey = true;
    }

    for (Door* door : doors) {
        if (door != nullptr && !door->isOpen() && player->intersects(door, 1, 1, 1, 1)) {
            door->open();
        }
    }

    if (hasKey) {
        for (Gate* gate : gates) {
            if (gate != nullptr && !gate->isOpen() && player->intersects(gate, 1, 1, 1, 1)) {
                gate->open();
            }
        }
    }
}
