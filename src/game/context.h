#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include <SDL3_image/SDL_image.h>

#include "game/client.h"

class Clock;
class Input;
class Camera;
class Screen;

class Context {
public:
    Context();
    ~Context();

    Clock* clock;
    Input* input;
    Camera* camera;
    Screen* screen;
    Client* client = nullptr;
    SDL_Renderer* renderer = nullptr;
};

#endif // GAME_CONTEXT_H


