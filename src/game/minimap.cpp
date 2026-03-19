#include <unordered_map>

#include <game/minimap.h>

#include "game/state.h"
#include "game/drag.h"
#include "game/cell.h"
#include "game/camera.h"
#include "game/input.h"
#include "game/screen.h"
#include "game/position.h"

// Grid width and grid height is needed here
// To initialize background texture
// Later we might directly render it(?)
// Better with server possibly
Minimap::Minimap(SDL_Renderer* renderer,
        int minimapWidth,
        int minimapHeight,
        int gridWidth,
        int gridHeight,
        float minimapScale
        ) {

    this->gridWidth = gridWidth;
    this->gridHeight = gridHeight;
    this->minimapScale = minimapScale;

    this->renderer = renderer;

    frame.x = 0;
    frame.y = 0;
    frame.w = minimapWidth;
    frame.h = minimapHeight;

    position = new Position(0,0);

    setSize(minimapWidth, minimapHeight);

    background = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        gridWidth * minimapScale,
        gridHeight * minimapScale);
    
    SDL_SetTextureBlendMode(background, SDL_BLENDMODE_NONE);


    foreground = SDL_CreateTexture(renderer, 
        SDL_PIXELFORMAT_RGBA32, 
        SDL_TEXTUREACCESS_TARGET, 
        frame.w, 
        frame.h);
    
    SDL_SetTextureBlendMode(foreground, SDL_BLENDMODE_BLEND);

    drag = new Drag(getPosition(), false, true);

    cooldown = new Cooldown(100);

}

// Map is linking to minimap using this function
// Basically it shares its grid and grid cell size
// 
void Minimap::setMapData(std::vector<std::vector<Cell*>>& grid, int cellWidth, int cellHeight) {
    this->grid = &grid;
    this->cellWidth = cellWidth;
    this->cellHeight = cellHeight;
}

// Map genrator or Map modifier calles this
// It should reflect terrain changes or fillings live
void Minimap::setTerrain(int cell, int row, int red, int green, int blue) {
    if (!background) return;

    SDL_SetRenderTarget(renderer, background);

    SDL_FRect rect;
    rect.x = cell * minimapScale;
    rect.y = row * minimapScale;
    rect.w = minimapScale;
    rect.h = minimapScale;

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderTarget(renderer, nullptr);
}



void Minimap::update(State* state) {
    
    Mouse* mouse = state->input->mouse;
    Camera* camera = state->camera;
    Screen* screen = state->screen;
    
    drag->update(state);

    // This allows to scroll map with scope drag
    bool manualPick = false;
    if ((mouse->leftClick || mouse->leftDragActive) && mouse->inside(getPosition())) {

        camera->setPosition(
            (((mouse->x - getX() + frame.x)) / minimapScale) * cellWidth - camera->getWidth()/2,
            (((mouse->y - getY() + frame.y)) / minimapScale) * cellHeight - camera->getHeight()/2
        );
        manualPick = true;
    }

    // resetBackgroundTexture scope rectangle
    scope.x = (camera->getX()/cellWidth) * minimapScale - frame.x + getX();
    scope.y = (camera->getY()/cellHeight) * minimapScale - frame.y + getY();
    scope.w = ((screen->getWidth()/cellWidth) * minimapScale) / camera->getZoom();
    scope.h = ((screen->getHeight()/cellHeight) * minimapScale) / camera->getZoom();

    if (!manualPick) {
        int oldFrameX = frame.x;
        int oldFrameY = frame.y;
        frame.x = ((camera->getX()/cellWidth) * minimapScale) - frame.w / 2;
        frame.y = ((camera->getY()/cellHeight) * minimapScale) - frame.h / 2;
        if (frame.x<0) {frame.x=0;};
        if (frame.y<0) {frame.y=0;};
        if ((frame.x+frame.w)/minimapScale>gridWidth) {frame.x=gridWidth*minimapScale-frame.w;};
        if ((frame.y+frame.h)/minimapScale>gridHeight) {frame.y=gridHeight*minimapScale-frame.h;};
        if (oldFrameX!=frame.x || oldFrameY!=frame.y) {
            modified = true;
        }
    }

    if (camera->getX()!=prevCameraX || 
        camera->getY()!=prevCameraY || 
        camera->getWidth()!=prevCameraWidth || 
        camera->getHeight()!=prevCameraHeight) {
        modified = true;
        prevCameraX = camera->getX();
        prevCameraY = camera->getY();
        prevCameraWidth = camera->getWidth();
        prevCameraHeight = camera->getHeight();        
    }

}


void Minimap::render(State* state) {
    if (!visible) {
        return;
    }

    SDL_RenderTexture(renderer, background, &frame, getPosition());
    // renderRectBorder(getPosition(), 102, 5, 51, 255);
    SDL_SetRenderDrawColor(renderer, 102, 5, 51, 255);
    SDL_RenderRect(renderer, getPosition());

    if (modified && cooldown->isReady()) {

        std::vector<SDL_FRect> blackBorders;
        std::unordered_map<SDL_Color*, std::vector<SDL_FRect>> rects;

        const int xCellFrom = std::max(0, int(frame.x / minimapScale) - 2);
        const int xCellTo = std::min(gridWidth, int(xCellFrom + frame.w / minimapScale)+4);
        const int yCellFrom = std::max(0, int(frame.y / minimapScale) - 2);
        const int yCellTo = std::min(gridHeight, int(yCellFrom + frame.h / minimapScale)+4);

        const float widthFactor = minimapScale / cellWidth;
        const float heightFactor = minimapScale / cellHeight;

        // Iterate over cells within the frame
        for (int i = xCellFrom; i < xCellTo; i++) {
            for (int j = yCellFrom; j < yCellTo; j++) {

                Cell* cell = (*grid)[i][j];

                for(auto& unitList : cell->units){
                    for(Unit* unit : unitList){

                        // Original rectangle
                        SDL_FRect rect = {
                            unit->getX() * widthFactor - frame.x,
                            unit->getY() * heightFactor - frame.y,
                            std::max(2.0f, unit->getWidth() * widthFactor),
                            std::max(2.0f, unit->getHeight() * heightFactor)
                        };

                        // Original rectangle
                        rects[unit->getColor()].push_back(rect);

                        // Black border rectangle
                        rect.x -= 1;
                        rect.y -= 1;
                        rect.w += 2;
                        rect.h += 2;
                        blackBorders.push_back(rect);
                    }
                }
            }
        }

        // New method
        SDL_SetRenderTarget(renderer, foreground);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // clear it
        SDL_RenderClear(renderer);

        // First render the black border effect
        // Uint32 blackColor = SDL_MapRGB(pixelFormatDetails, nullptr, 0, 0, 0);
        SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
        SDL_RenderFillRects (renderer, blackBorders.data(), blackBorders.size());
                
        // Then render unit colors
        for (auto& [c, items] : rects) {
            // Uint8 r, g, b;
            // SDL_GetRGB(color, pixelFormatDetails, nullptr, &r, &g, &b);
            SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, 255); // fully opaque
            SDL_RenderFillRects(renderer, items.data(), items.size());
        }

    // Scope rectangle color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 70);
    SDL_RenderFillRect(renderer, &scope);

    // Scope rectangle border
    SDL_SetRenderDrawColor(renderer, 102, 5, 51, 255);
    SDL_RenderRect(renderer, &scope);

        SDL_SetRenderTarget(renderer, nullptr); // back to default        
        
        // SDL_UnlockTexture(foreground);
        cooldown->reset();
        modified = false;
        // modified = false;
    }

    SDL_RenderTexture(renderer, foreground, NULL, getPosition());



    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}



bool Minimap::isVisible(State* state) {
    return visible;
}

void Minimap::toggleVisible() {
    visible = !visible;
}
void Minimap::markModified() {
    modified = true;
}
