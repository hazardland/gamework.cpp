#ifndef GAME_DEBUG_H
#define GAME_DEBUG_H

#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Context;
#include "game/position.h"

void debug(Context* context, TTF_Font* font = nullptr);
void draw(SDL_FRect* rect, SDL_Color color = {161, 195, 69, 255});

inline void draw(Position* position, SDL_Color color = {161, 195, 69, 255});

void printLines(SDL_FRect* rect, const std::vector<std::string>& lines);

namespace debug_detail {

inline std::string toString(bool value) {
    return value ? "true" : "false";
}

inline std::string toString(const std::string& value) {
    return value;
}

inline std::string toString(const char* value) {
    return value == nullptr ? "" : std::string(value);
}

template<typename T>
std::string toString(const T& value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

inline void collect(std::vector<std::string>& lines) {
}

template<typename Label, typename Value, typename... Rest>
void collect(std::vector<std::string>& lines, Label&& label, Value&& value, Rest&&... rest) {
    lines.push_back(
        toString(std::forward<Label>(label)) + ": " + toString(std::forward<Value>(value))
    );
    collect(lines, std::forward<Rest>(rest)...);
}

}

template<typename... Args>
void print(SDL_FRect* rect, Args&&... args) {
    static_assert(sizeof...(args) % 2 == 0, "print expects label/value pairs");
    std::vector<std::string> lines;
    lines.reserve(sizeof...(args) / 2);
    debug_detail::collect(lines, std::forward<Args>(args)...);
    printLines(rect, lines);
}

template<typename... Args>
void print(Position* position, Args&&... args) {
    static_assert(sizeof...(args) % 2 == 0, "print expects label/value pairs");
    print(position->getPosition(), std::forward<Args>(args)...);
}

inline void draw(Position* position, SDL_Color color) {
    draw(position->getPosition(), color);
}

#endif // GAME_DEBUG_H


