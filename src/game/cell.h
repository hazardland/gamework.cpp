#ifndef GAME_CELL_H
#define GAME_CELL_H

#include <list>
#include <vector>

#include <SDL3_image/SDL_image.h>

class Unit;

class Cell {
    public:
        int tile;
        int type;
        SDL_FRect* rect;

        std::vector<std::list<Unit*>> units;

        Cell(int layers); // The 'objects' vector is initialized with 'layers' number of empty lists.
};

#endif


