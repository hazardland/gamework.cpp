#include "game/map.h"

#include "game/object.h"
#include "game/image.h"
#include "game/clip.h"
#include "game/state.h"
#include "game/text.h"
#include "game/terrain.h"
#include "game/minimap.h"
#include "game/cell.h"
#include "game/noise.h"
#include "game/frame.h"
#include "game/camera.h"
#include "game/noise.h"

Map::Map(Image* image,
         int cellWidth,
         int cellHeight,
         int gridWidth,
         int gridHeight,
         int layerCount,
         TTF_Font* font) {

    this->image = image;
    this->cellWidth = cellWidth;
    this->cellHeight = cellHeight;
    this->gridWidth = gridWidth;
    this->gridHeight = gridHeight;
    clip = new Clip(image,
                        cellWidth,
                        cellHeight,
                        1, 1,
                        (image->getWidth()/cellWidth)*(image->getHeight()/cellHeight));

   grid.resize(gridWidth, std::vector<Cell*>(gridHeight, nullptr));
    // grid.resize(gridWidth, std::vector<std::unique_ptr<Cell>>(gridHeight));

    for (int x = 0; x < gridWidth; x++)
    {
        //grid.push_back(std::vector<Cell*>());
        for (int y = 0; y < gridHeight; y++) {
            //grid[x].push_back(new Cell(layerCount));
            grid[x][y] = new Cell(layerCount);
            // grid[x][y] = std::make_unique<Cell>(layerCount);


        }
    }
    this->text = new Text(font);
    this->text->setColor(SDL_Color(255, 255, 255))->enableCache();

}


float Map::getWidth() {
    return gridWidth*cellWidth;
}

float Map::getHeight() {
    return gridHeight*cellHeight;
}

void Map::render(State* state) {

    Camera* camera = state->camera;

    // Choose renderable tiles
    // @todo Code can be optimized furzer
    int xCellFrom = (camera->getX() / cellWidth);
    int xCellTo = (camera->getWidth() / cellWidth) + xCellFrom + 2;
    int yCellFrom = (camera->getY() / cellHeight);
    int yCellTo = (camera->getHeight() / cellHeight) + yCellFrom + 2;
    if (xCellFrom<0) xCellFrom = 0;
    if (yCellFrom<0) yCellFrom = 0;
    if (xCellTo>gridWidth) xCellTo = gridWidth;
    if (yCellTo>gridHeight) yCellTo = gridHeight;
    //printf("map render region: %d,%d x %d,%d\n", xCellFrom, yCellFrom, xCellTo, yCellTo);

    SDL_FRect location;
    location.w = cellWidth;
    location.h = cellHeight;
    SDL_FRect* position;

    for (int x = xCellFrom; x < xCellTo; x++)
    {
        for (int y = yCellFrom; y < yCellTo; y++)
        {
            //grid[x][y]
            location.x = x*cellWidth;
            location.y = y*cellHeight;
            if (state->camera->isVisible(&location)) {
                position = state->camera->translate(&location);
                image->render(grid[x][y]->rect, position);

                // Debug
                if (debug){

                    // printf("%d ", clip->getFrame(grid[x][y])->rect.x);
                    SDL_SetRenderDrawColor(image->renderer, 0, 0, 0, 255);
                    SDL_RenderRect(image->renderer, position);
                    SDL_SetRenderDrawColor(image->renderer, 0, 0, 0, 0);

                    std::array<int, 4> borders = getTileBorders(x, y);

                    text->setText(std::to_string(borders[0])); //+"tl"
                    text->setPosition(position->x+1, position->y+1);
                    text->render(state);

                    text->setText(std::to_string(borders[1])); //+"tr"
                    text->setPosition(position->x+position->w-text->getWidth()-1, position->y+1);
                    text->render(state);

                    text->setText(std::to_string(borders[2])); //+"bl"
                    text->setPosition(position->x+1, position->y+position->h-text->getHeight()-1);
                    text->render(state);

                    text->setText(std::to_string(borders[3])); //+"br"
                    text->setPosition(position->x+position->w-text->getWidth()-1, position->y+position->h-text->getHeight()-1);
                    text->render(state);
                }
            }
        }
    }
}

// Implement setTerrain method
void Map::setTerrain(int x, int y, int type) {
    Terrain* terrain = terrains[type];
    grid[x][y]->terrain = terrain;
    if (minimap!=nullptr) {
        minimap->setTerrain(x, y, terrain->color[0], terrain->color[1], terrain->color[2]);
    }
}

void Map::setCell(int x, int y, int terrain, int tile) {
    setTerrain(x, y, terrain);
    grid[x][y]->tile = tile;
    grid[x][y]->rect = &clip->getFrame(tile)->rect;
}

void Map::setMinimap(Minimap* minimap) {
    this->minimap = minimap;
    minimap->setMapData(grid, cellWidth, cellHeight);
    // minimap->updateMapSize(getWidth(), getHeight(), tileScale);
}


// void Map::import(std::vector<std::vector<int>> data) {

void Map::import(std::vector<std::vector<int>> data) {
    int height = data.size();
    int width = data[0].size();
    for (int y=0; y<height; y++){
        for (int x=0; x<width; x++)
        {
            setTerrain(x, y, data[y][x]);
        }
    }
    //map->grid = this->grid;
    // printf("imported custom map\n");
    fillMap();
}

void Map::generate(
    int seed,
    float intensity,
    std::vector<float> ranges) {

    OpenSimplexNoise::Noise noise{seed};

    for (int x = 0; x < gridWidth / 2; x++) {
        for (int y = 0; y < gridHeight / 2; y++) {

            // Compute noise value at scaled coordinates
            float depth = (noise.eval(x * intensity, y * intensity) + 1) / 2.0;
            int terrain = 0; // Default terrain type

            // Determine terrain type based on depth value
            for (int t = 0; t < terrains.size(); t++) {
                if (depth <= ranges[t]) {
                    terrain = t;
                    break;
                }
            }

            // Map the smaller grid space to a 2x2 block in the full grid
            int realX = x * 2;
            int realY = y * 2;
            setTerrain(realX, realY, terrain);
            setTerrain(realX + 1, realY, terrain);
            setTerrain(realX, realY + 1, terrain);
            setTerrain(realX + 1, realY + 1, terrain);
        }
    }

    printf("Generated map with 2x2 terrain blocks\n");
    fillMap();
}

// void Map::generate(
//                 int seed,
//                 float intensity,
//                 std::vector<float> ranges) {

//     OpenSimplexNoise::Noise noise{seed};
//     for (int x=0; x<gridWidth; x+=2) {
//         for (int y=0; y<gridHeight; y+=2)
//         {
//             // alpha = (noise.eval(x*0.01, y*0.01) + 1) / 2.0  * 255.0;
//             // minimap->setPixel(x, y, 255, 255, 255, alpha);
//             float depth = (noise.eval(x*intensity, y*intensity) + 1) / 2.0;
//             for (int terrain=0; terrain<terrains.size(); terrain++){
//                 if (depth<=ranges[terrain]) {
//                     setTerrain(x, y, terrain);
//                     setTerrain(x+1, y, terrain);
//                     setTerrain(x, y+1, terrain);
//                     setTerrain(x+1, y+1, terrain);
//                     break;
//                 }
//             }
//         }
//     }
//     // fillMap2();
//     // map->grid = this->grid;
//     printf("generated map 2\n");
//     fillMap();

// }

void Map::fillMap() {

    for (int x=0; x<gridWidth; x++) {
        for (int y=0; y<gridHeight; y++) {
            grid[x][y]->tile = calculateTile(x, y);
            grid[x][y]->rect = &clip->getFrame(grid[x][y]->tile)->rect;
        }
    }
    printf("filled map 2\n");
}

int Map::getTile(const std::array<int, 4>& corners) {
    int key = corners[0] * 1000 + corners[1] * 100 + corners[2] * 10 + corners[3];
    auto it = tiles.find(key);

    if (it == tiles.end() || it->second.empty()) {
        printf(
            "Map::getTile missing key %d for corners {%d,%d,%d,%d}\n",
            key,
            corners[0],
            corners[1],
            corners[2],
            corners[3]
        );
        return 0;
    }

    return it->second[rand_(0, it->second.size() - 1)];
}

int Map::calculateTile(int x, int y) {
    std::array<int, 4> borders = getTileBorders(x, y);
    // std::cout << "bord: " << borders[0] << borders[1] << borders[2] << borders[3] << std::endl;

    if (borders == std::array<int, 4>{1, 1, 1, 1}) {
        return getTile(borders);
    }
    return getTile(borders);
}

// In a cold winter I made this algorithm on a paper
// Everybody had fever at home
// This scans neigboars and genrates an array [*,*,*,*]
// Where each element encoded terrain type for 4 corners of tile (from top left clockwise)
// This is for smooth terrain transitions
std::array<int, 4> Map::getTileBorders(int x, int y) {
    // Get corner sum
    Cell* cell = this->grid[x][y];
    int type = cell->terrain->id;
    // Terrain* terrain = cell->terrain;

    if (type == 0) {
        return {0, 0, 0, 0}; // Simple return for type 0
    }
    //   x x x
    // y 0 0 0
    // y 0 0 0
    // y 0 0 0
    int borders[] = {
        type, type, type, // 0 1 2
        type, type, type, // 3 4 5
        type, type, type  // 6 7 8
    };
    int corners[] = {
        0, 0, // 0 1
        0, 0  // 2 3
    };

    if (x!=0 && y!=0){
        borders[0] = this->grid[x-1][y-1]->terrain->id < type ? type-1 : type;
    }
    if (y!=0){
        borders[1] = this->grid[x  ][y-1]->terrain->id < type ? type-1 : type;
    }
    if (y!=0 && x<gridWidth-1) {
        borders[2] = this->grid[x+1][y-1]->terrain->id < type ? type-1 : type;
    }
    if (x!=0) {
        borders[3] = this->grid[x-1][y  ]->terrain->id < type ? type-1 : type;
    }
    if (x<gridWidth-1) {
        borders[5] = this->grid[x+1][y  ]->terrain->id < type ? type-1 : type;
    }
    if (x!=0 && y<gridHeight-1) {
        borders[6] = this->grid[x-1][y+1]->terrain->id < type ? type-1 : type;
    }
    if (y<gridHeight-1){
        borders[7] = this->grid[x  ][y+1]->terrain->id < type ? type-1 : type;
    }
    if (x<gridWidth-1 && y<gridHeight-1) {
        borders[8] = this->grid[x+1][y+1]->terrain->id < type ? type-1 : type;
    }

    corners[0] = borders[3] + borders[0] + borders[1];
    corners[1] = borders[1] + borders[2] + borders[5];
    corners[2] = borders[3] + borders[6] + borders[7];
    corners[3] = borders[7] + borders[8] + borders[5];

    int corner = 0;
    int min = corners[0];

    for(int i=1; i<4; i++)
    {
        if(corners[i]<min)
        {
            min = corners[i];
            corner = i;
        }
    }

    // Return the selected four border values based on the determined corner
    switch (corner) {
        case 0:
            return {borders[0], borders[1], borders[3], borders[4]};
        case 1:
            return {borders[1], borders[2], borders[4], borders[5]};
        case 2:
            return {borders[3], borders[4], borders[6], borders[7]};
        case 3:
            return {borders[4], borders[5], borders[7], borders[8]};
    }

    return {0, 0, 0, 0}; // Fallback (should never be reached)
}

int Map::rand_(int min, int max) {
    return rand() % (max-min+1) + min;
}

void Map::setDebug (bool value) {
    debug = true;
}

void Map::toggleDebug () {
    debug = !debug;
}

void Map::markModified() {
    if (minimap!=nullptr){
        this->minimap->markModified();
    }
}

Map::~Map() {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            delete grid[x][y];  // Free each Cell*
        }
    }
}
