#ifndef GAME_MINIMAP_H
#define GAME_MINIMAP_H

#include <vector>

#include <SDL3_image/SDL_image.h>

#include "game/unit.h"
#include "game/cooldown.h"

class Drag;
class Cell;
class State;

class Minimap: public Unit {
public:
    Minimap(SDL_Renderer* renderer,
           int minimapWidth,
           int minimapHeight,
           int gridWidth,
           int gridHeight,
           float minimapScale
    );

    void setTerrain(int x, int y, int red, int blue, int green);
    void setMapData(std::vector<std::vector<Cell*>>& grid, int cellWidth, int cellHeight);
    virtual void update(State* state) override;
    virtual void render(State* state) override;
    virtual bool isVisible(State* state) override;
    void toggleVisible();
    void markModified();

    private:
    std::vector<std::vector<Cell*>>* grid;
    float cellWidth;
    float cellHeight;
    int gridWidth;
    int gridHeight;
    float minimapScale;
    bool modified = true;
    SDL_Renderer* renderer;
    SDL_Texture* background = NULL;
    SDL_Texture* foreground = NULL;
    SDL_FRect scope;
    SDL_FRect frame;
    std::vector<Unit*> units;
    SDL_Color scopeColor = {50, 205, 50};
    Drag* drag;
    Cooldown* cooldown;
    bool visible = true;
    int prevCameraX;
    int prevCameraY;
    int prevCameraWidth;
    int prevCameraHeight;
};

#endif // GAME_MINIMAP_H
