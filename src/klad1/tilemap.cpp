#include "klad1/tilemap.h"

#include "game/state.h"

TileMap::TileMap(Image* image, int tileWidth, int tileHeight, const Level* level) {
    this->image = image;
    this->tileWidth = tileWidth;
    this->tileHeight = tileHeight;
    this->level = level;
    columns = image->getWidth() / tileWidth;
}

void TileMap::render(State* state) {
    SDL_FRect source;
    source.w = tileWidth;
    source.h = tileHeight;

    SDL_FRect target;
    target.w = tileWidth;
    target.h = tileHeight;

    for (int y = 0; y < Level::height; ++y) {
        for (int x = 0; x < Level::width; ++x) {
            const int tile = level->tiles[y][x];

            source.x = (tile % columns) * tileWidth;
            source.y = (tile / columns) * tileHeight;

            target.x = x * tileWidth;
            target.y = y * tileHeight;

            image->render(&source, &target);
        }
    }
}

TileMap::~TileMap() {
    delete image;
}
