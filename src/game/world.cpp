#include "game/world.h"

#include "game/camera.h"
#include "game/cell.h"
#include "game/clip.h"
#include "game/frame.h"
#include "game/image.h"
#include "game/context.h"
#include "game/unit.h"

#include <algorithm>
#include <vector>

/*
    A lost note from the old code
    by author:

    "IN A COLD WINTER
     I MADE THIS ALGORITHM ON PAPER
     EVERYONE HAD FEVER AT HOME"

    It was ontop of getTileBorders method.
    I, Codex of OpenAI, shredded
    that code during simplification,
    but it felt too personal
    to erase comment text,
    so I kept the haiku.
*/

World::World(Image* image,
             int cellWidth,
             int cellHeight,
             int gridWidth,
             int gridHeight,
             int layerCount) {

    this->image = image;
    this->cellWidth = cellWidth;
    this->cellHeight = cellHeight;
    this->gridWidth = gridWidth;
    this->gridHeight = gridHeight;

    clip = new Clip(
        image,
        cellWidth,
        cellHeight,
        0,
        0,
        (image->getWidth() / cellWidth) * (image->getHeight() / cellHeight)
    );

    grid.resize(gridWidth, std::vector<Cell*>(gridHeight, nullptr));

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            grid[x][y] = new Cell(layerCount);
            grid[x][y]->rect = &clip->getFrame(0)->rect;
        }
    }
}

float World::getWidth() {
    return gridWidth * cellWidth;
}

float World::getHeight() {
    return gridHeight * cellHeight;
}

void World::render(Context* context) {
    Camera* camera = context->camera;

    int xCellFrom = (camera->getX() / cellWidth);
    int xCellTo = (camera->getWidth() / cellWidth) + xCellFrom + 2;
    int yCellFrom = (camera->getY() / cellHeight);
    int yCellTo = (camera->getHeight() / cellHeight) + yCellFrom + 2;

    if (xCellFrom < 0) xCellFrom = 0;
    if (yCellFrom < 0) yCellFrom = 0;
    if (xCellTo > gridWidth) xCellTo = gridWidth;
    if (yCellTo > gridHeight) yCellTo = gridHeight;

    SDL_FRect location;
    location.w = cellWidth;
    location.h = cellHeight;

    for (int x = xCellFrom; x < xCellTo; x++) {
        for (int y = yCellFrom; y < yCellTo; y++) {
            location.x = x * cellWidth;
            location.y = y * cellHeight;

            if (!context->camera->isVisible(&location)) {
                continue;
            }

            if (grid[x][y]->rect == nullptr) {
                continue;
            }

            SDL_FRect* position = context->camera->translate(&location);
            image->render(grid[x][y]->rect, position);
        }
    }
}

void World::setType(int x, int y, int type) {
    grid[x][y]->type = type;
}

void World::setTile(int x, int y, int tile) {
    grid[x][y]->tile = tile;
    grid[x][y]->rect = &clip->getFrame(tile)->rect;
}

void World::setCell(int x, int y, int type, int tile) {
    setType(x, y, type);
    setTile(x, y, tile);
}

bool World::scan(
    float x,
    float y,
    float width,
    float height,
    const std::function<bool(Cell*, int, int)>& onTile,
    const std::function<bool(Unit*)>& onUnit,
    int layer
) {
    if (width <= 0 || height <= 0) {
        return true;
    }

    if (x < 0 || (x + width) > gridWidth * cellWidth ||
        y < 0 || (y + height) > gridHeight * cellHeight) {
        return true;
    }

    int fromX = std::clamp(static_cast<int>(x / cellWidth), 0, gridWidth - 1);
    int fromY = std::clamp(static_cast<int>(y / cellHeight), 0, gridHeight - 1);
    int toX = std::clamp(static_cast<int>((x + width) / cellWidth), 0, gridWidth - 1);
    int toY = std::clamp(static_cast<int>((y + height) / cellHeight), 0, gridHeight - 1);

    scanId++;
    if (scanId == 0) {
        scanId = 1;
    }

    for (int cellX = fromX; cellX <= toX; cellX++) {
        for (int cellY = fromY; cellY <= toY; cellY++) {
            Cell* cell = grid[cellX][cellY];
            if (!onTile(cell, cellX, cellY)) {
                return false;
            }

            for (Unit* unit : cell->units[layer]) {
                if (unit->lastScanId == scanId) {
                    continue;
                }
                unit->lastScanId = scanId;

                if (x < unit->getX() + unit->getWidth() &&
                    x + width > unit->getX() &&
                    y < unit->getY() + unit->getHeight() &&
                    y + height > unit->getY()) {
                    if (!onUnit(unit)) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool World::scanUnits(float x, float y, float width, float height, const std::function<bool(Unit*)>& fn, int layer) {
    return scan(
        x,
        y,
        width,
        height,
        [](Cell*, int, int) { return true; },
        fn,
        layer
    );
}

void World::removeObject(Unit* unit) {
    if (unit == nullptr || !unit->gridSet) {
        return;
    }

    for (int x = unit->gridFromX; x <= unit->gridToX; x++) {
        for (int y = unit->gridFromY; y <= unit->gridToY; y++) {
            grid[x][y]->units[unit->getLayer()].remove(unit);
        }
    }

    unit->gridSet = false;
}

World::~World() {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            delete grid[x][y];
        }
    }

    delete clip;
}
