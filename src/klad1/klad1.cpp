#include "klad1/klad1.h"
#include "klad1/bridge.h"
#include "klad1/brick.h"
#include "klad1/door.h"
#include "klad1/gold.h"
#include "klad1/ladder.h"
#include "klad1/level.h"
#include "klad1/levels/level1.h"
#include "klad1/player.h"
#include "klad1/tide.h"

#include "game/camera.h"
#include "game/clip.h"
#include "game/frame.h"
#include "game/image.h"
#include "game/world.h"
#include "game/sprite.h"
#include "game/context.h"

#include <SDL3_ttf/SDL_ttf.h>

void Klad1::prepare(Context* context) {
    Scene::prepare(context);

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

    sprites[SPRITE_BRICK] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 8, 0, 1, false, false);
    sprites[SPRITE_BRIDGE] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 7, 0, 1, false, false);
    sprites[SPRITE_LADDER] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 6, 0, 1, false, false);
    sprites[SPRITE_GOLD] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 1, 0, 1, false, false);
    sprites[SPRITE_DOOR] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 2, 0, 1, false, false);
    sprites[SPRITE_TIDE] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 4, 0, 1, false, false);
    sprites[SPRITE_PLAYER] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT, 133))
        ->addClip(1, 24, 0, 1, false, false)
        ->addClip(2, 25, 0, 2, false, false)
        ->addClip(3, 26, 0, 4, false, false)
        ->addClip(4, 26, 0, 4, true, false);

    Clip* climbClip = sprites[SPRITE_PLAYER]->getClip(2);
    climbClip->frames[1].rect = climbClip->frames[0].rect;
    climbClip->frames[1].flipX = true;
    climbClip->frames[1].flipY = false;
    climbClip->frames[1].cacheFlip();

    for (int y = 0; y < Level::HEIGHT; y++) {
        for (int x = 0; x < Level::WIDTH; x++) {
            uint8_t value = level1.grid[y][x];
            world->setCell(x, y, TILE_BLANK, Level::BLANK);

            switch (value) {
                case Level::GOLD: {
                    Gold* gold = new Gold(
                        sprites[SPRITE_GOLD],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    addObject(gold);
                    break;
                }

                case Level::EXIT:
                case Level::DOOR: {
                    Door* door = new Door(
                        sprites[SPRITE_DOOR],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    addObject(door);
                    break;
                }

                case Level::TIDE: {
                    Tide* tide = new Tide(
                        sprites[SPRITE_TIDE],
                        x * CELL_WIDTH,
                        y * CELL_HEIGHT
                    );
                    addObject(tide);
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

    addFps(font);

    context->camera->setZoom(1.5f);
}

void Klad1::update(Context* context) {
    Scene::update(context);
}
