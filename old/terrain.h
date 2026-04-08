// File: terrain.h

#ifndef GAME_TERRAIN_H
#define GAME_TERRAIN_H

#include <vector>

/**
 * @brief The Terrain class represents a terrain type in the game.
 *
 * This class stores properties related to a specific terrain such as its id, layer and color.
 */
class Terrain {
public:
    // Terrain id
    int id;

    // Layer in which terrain is present
    int layer;

    // Color of the terrain
    std::vector<int> color;

    // Functions declaration
    Terrain(int id, int layer, std::vector<int> color);
};

#endif // GAME_TERRAIN_H


