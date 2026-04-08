#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <functional>
#include <vector>

#include "game/object.h"

class Cell;
class Clip;
class Image;
class Context;
class Unit;

class World : public Object {
private:
    Image* image;
    Clip* clip;

public:
    int cellWidth;
    int cellHeight;
    int gridWidth;
    int gridHeight;
    std::vector<std::vector<Cell*>> grid;
    uint64_t scanId = 0;

    World(Image* image, int cellWidth, int cellHeight, int gridWidth, int gridHeight, int layerCount = 3);
    void render(Context* context) override;
    float getWidth();
    float getHeight();
    void setType(int x, int y, int type);
    void setTile(int x, int y, int tile);
    void setCell(int x, int y, int type, int tile);
    bool scanUnits(float x, float y, float width, float height, const std::function<bool(Unit*)>& fn, int layer = -1);

    ~World();
};

#endif // GAME_WORLD_H


