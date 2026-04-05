#pragma once

#include <array>
#include <cstdint>

struct Position {
    uint8_t x;
    uint8_t y;
};

struct Level {
    static constexpr int WIDTH = 32;
    static constexpr int HEIGHT = 22;

    int id;
    Position exit;
    Position playerSpawn;
    std::array<Position, 2> enemySpawns;
    bool hasBonusLife;
    std::array<uint8_t, 6> keyGolds;
    uint8_t unknown;
    std::array<std::array<uint8_t, WIDTH>, HEIGHT> tiles;
};
