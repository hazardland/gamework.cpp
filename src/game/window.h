#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "game/cooldown.h"

class Context;
class Scene;

extern bool SDL_STARTED;

class Window {

private:
    SDL_Window* window = nullptr;
    Scene* scene = nullptr;
    Context* context = nullptr;
    bool fullscreen = false;
    Cooldown* fullscreenCooldown = nullptr;
    int originalWidth = 0;
    int originalHeight = 0;

public:
    Window(const char* title, int width, int height, Context* context = nullptr);
    void setScene(Scene* scene);
    void onResize(int width, int height);
    SDL_Window* getInstance();
    void toggleFullscreen();
    bool isFullscreen();
    int run();
    ~Window();
};

#endif // GAME_WINDOW_H


