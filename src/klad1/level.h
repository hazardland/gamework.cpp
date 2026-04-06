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

    static constexpr int BLANK = 0;
    static constexpr int GOLD = 1;
    static constexpr int EXIT = 2;
    static constexpr int DOOR = 3;
    static constexpr int TIDE = 4;
    static constexpr int WATER = 5;
    static constexpr int LADDER = 6;
    static constexpr int BRIDGE = 7;
    static constexpr int BRICK = 8;
    static constexpr int WALL = 9;

    int id;
    Position exit;
    Position playerSpawn;
    std::array<Position, 2> enemySpawns;
    bool hasBonusLife;
    std::array<uint8_t, 6> keyGolds;
    uint8_t unknown;
    std::array<std::array<uint8_t, WIDTH>, HEIGHT> grid;
};
