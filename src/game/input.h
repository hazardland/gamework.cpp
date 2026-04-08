#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <SDL3/SDL.h>

#include "game/keyboard.h"
#include "game/mouse.h"

class Window;

class Input {
    public:
        bool close = false;
        Keyboard* keyboard = new Keyboard();
        Mouse* mouse = new Mouse();
        void setWindow(Window* window);
        void poll();
        ~Input();
    private:
        void handle(SDL_Event& event);
        Window* window = nullptr;
};

#endif


