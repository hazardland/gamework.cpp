#include "game/cell.h"


// #include <game/context.h>
#include "game/unit.h"

Cell::Cell(int layers) : units(layers) {
    tile = 0;
    type = 0;
    rect = nullptr;
}


