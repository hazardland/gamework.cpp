#pragma once

#include "game/image.h"
#include "klad1/level.h"

class State;

class TileMap {
public:
    TileMap(Image* image, int tileWidth, int tileHeight, const Level* level);
    ~TileMap();

    void render(State* state);

private:
    Image* image;
    const Level* level;
    int tileWidth;
    int tileHeight;
    int columns;
};
