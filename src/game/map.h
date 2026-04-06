#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <stdio.h>
#include <vector>
#include <map>

#include "game/object.h"
#include "game/text.h"

// class Object;
class Text;
class Minimap;
class Terrain;
class Cell;
class Image;
class Clip;
class State;

class Map : public Object {
private:
    bool debug = false;
    Image* image;
    Clip* clip;
    Text* text;
    Minimap* minimap = nullptr;

    int rand_(int min, int max);
    int getTile(const std::array<int, 4>& corners);
    std::array<int, 4> getTileBorders(int x, int y);
    int calculateTile(int x, int y);

public:

    int cellWidth;
    int cellHeight;
    int gridWidth;
    int gridHeight;
    std::vector<std::vector<Cell*>> grid;
    // std::vector<std::vector<std::unique_ptr<Cell>>> grid;

    std::vector<Terrain*> terrains;
    std::map<int, std::vector<int>> tiles;

    Map(Image* image, int cellWidth, int cellHeight, int gridWidth, int gridHeight, int layerCount=3, TTF_Font* font = NULL);
    virtual void render(State* state);
    float getWidth();
    float getHeight();
    void setMinimap(Minimap* minimap);
    void setCell(int x, int y, int terrain, int tile);
    void setTerrain(int x, int y, int type);
    void fillMap();
    void import(std::vector<std::vector<int>> data);
    void generate(int seed, float intensity, std::vector<float> ranges);
    void setDebug(bool value);
    void toggleDebug();
    void markModified();
    ~Map();

private:

};

#endif // GAME_MAP_H
