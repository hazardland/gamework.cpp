#pragma once

#include <cstdint>

constexpr uint8_t EMPTY = 0;
constexpr uint8_t GOLD = 1;
constexpr uint8_t EXIT = 2;
constexpr uint8_t CLOSED_DOOR = 3;
constexpr uint8_t WATER_TOP = 4;
constexpr uint8_t WATER = 5;
constexpr uint8_t LADDER = 6;
constexpr uint8_t BRIDGE = 7;
constexpr uint8_t BRICK = 8;
constexpr uint8_t WALL = 9;

inline bool isSolid(uint8_t tile) {
    return tile == CLOSED_DOOR || tile == BRICK || tile == WALL;
}

inline bool isClimbable(uint8_t tile) {
    return tile == LADDER;
}

inline bool isCollectible(uint8_t tile) {
    return tile == GOLD;
}
