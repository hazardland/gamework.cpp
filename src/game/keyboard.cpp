#include "game/keyboard.h"

void Keyboard::read(const bool* keys) {
    this->keys = keys;

    up = keys[SDL_SCANCODE_UP];
    down = keys[SDL_SCANCODE_DOWN];
    right = keys[SDL_SCANCODE_RIGHT];
    left = keys[SDL_SCANCODE_LEFT];
    space = keys[SDL_SCANCODE_SPACE];
    plus = keys[SDL_SCANCODE_EQUALS];
    minus = keys[SDL_SCANCODE_MINUS];
    alt = keys[SDL_SCANCODE_LALT] || keys[SDL_SCANCODE_RALT];
    enter = keys[SDL_SCANCODE_RETURN];
    a = keys[SDL_SCANCODE_A];
    w = keys[SDL_SCANCODE_W];
    s = keys[SDL_SCANCODE_S];
    d = keys[SDL_SCANCODE_D];
    f5 = keys[SDL_SCANCODE_F5];
    f11 = keys[SDL_SCANCODE_F11];
    f12 = keys[SDL_SCANCODE_F12];
    tab = keys[SDL_SCANCODE_TAB];
}


