#ifndef GAME_MINIMAP_H
#define GAME_MINIMAP_H

#include <vector>

#include <SDL3_image/SDL_image.h>

#include "game/unit.h"
#include "game/cooldown.h"

class Drag;
class Cell;
class Context;

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
    virtual void update(Context* context) override;
    virtual void render(Context* context) override;
    virtual bool isVisible(Context* context) override;
    void toggleVisible();
    void markModified();
    ~Minimap() override;

    private:
    std::vector<std::vector<Cell*>>* grid = nullptr;
    float cellWidth = 0;
    float cellHeight = 0;
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
    Drag* drag = nullptr;
    Cooldown* cooldown = nullptr;
    bool visible = true;
    int prevCameraX = 0;
    int prevCameraY = 0;
    int prevCameraWidth = 0;
    int prevCameraHeight = 0;
};

#endif // GAME_MINIMAP_H


