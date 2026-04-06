#include "klad1/klad1.h"
#include "klad1/brick.h"
#include "klad1/level.h"
#include "klad1/levels/level1.h"
#include "klad1/player.h"

#include "game/camera.h"
#include "game/clip.h"
#include "game/frame.h"
#include "game/image.h"
#include "game/map.h"
#include "game/sprite.h"
#include "game/state.h"
#include "game/terrain.h"

#include <SDL3_ttf/SDL_ttf.h>

void Klad1::prepare(State* state) {
    Scene::prepare(state);

    image = new Image(renderer, "assets/klad1/sprites.png");
    font = TTF_OpenFont("assets/war2/fonts/titillium.ttf", 12);
    TTF_SetFontOutline(font, 1);

    map = new Map(
        image,
        CELL_WIDTH,
        CELL_HEIGHT,
        Level::WIDTH,
        Level::HEIGHT,
        2,
        font
    );

    map->terrains = {
        new Terrain(TERRAIN_BLANK, LAYER_WORLD, {0, 0, 0}),
        new Terrain(TERRAIN_WATER, LAYER_BACKGROUND, {0, 0, 0}),
        new Terrain(TERRAIN_WALL, LAYER_WORLD, {0, 0, 0}),
        new Terrain(TERRAIN_LADDER, LAYER_WORLD, {0, 0, 0})
    };

    sprites[SPRITE_BRICK] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 9, 1, 1, false, false);
    sprites[SPRITE_PLAYER] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT, 133))
        ->addClip(1, 25, 1, 1, false, false)
        ->addClip(2, 26, 1, 2, false, false)
        ->addClip(3, 27, 1, 4, false, false)
        ->addClip(4, 27, 1, 4, true, false);

    Clip* climbClip = sprites[SPRITE_PLAYER]->getClip(2);
    climbClip->frames[1].rect = climbClip->frames[0].rect;
    climbClip->frames[1].flipX = true;
    climbClip->frames[1].flipY = false;
    climbClip->frames[1].cacheFlip();

    for (int y = 0; y < Level::HEIGHT; y++) {
        for (int x = 0; x < Level::WIDTH; x++) {
            uint8_t value = level1.grid[y][x];
            map->setCell(x, y, TERRAIN_BLANK, Level::BLANK);

            switch (value) {
                case Level::WATER:
                    map->setCell(x, y, TERRAIN_WATER, Level::WATER);
                    break;

                case Level::BRICK: {
                    Brick* brick = new Brick(sprites[SPRITE_BRICK]);
                    brick->setMap(map);
                    brick->setPosition(x * CELL_WIDTH, y * CELL_HEIGHT);
                    addObject(brick);
                    break;
                }

                case Level::LADDER:
                    map->setCell(x, y, TERRAIN_LADDER, Level::LADDER);
                    break;

                case Level::BRIDGE:
                    map->setCell(x, y, TERRAIN_BLANK, Level::BRIDGE);
                    break;

                case Level::WALL:
                    map->setCell(x, y, TERRAIN_WALL, Level::WALL);
                    break;

                default:
                    break;
            }
        }
    }

    player = new Player(sprites[SPRITE_PLAYER], font);
    player->setMap(map);
    player->allowTerrain(TERRAIN_LADDER);
    player->setPosition(
        level1.playerSpawn.x * CELL_WIDTH,
        level1.playerSpawn.y * CELL_HEIGHT
    );
    addObject(player);

    state->camera->setZoom(1.5f);
}

void Klad1::update(State* state) {
    Scene::update(state);
}
