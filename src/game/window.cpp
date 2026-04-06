// File: window.cpp

#include <iostream>
// #include <SDL3/SDL.h>

#include "game/window.h"


#include "game/scene.h"
#include "game/state.h"
#include "game/state.h"
#include "game/input.h"
#include "game/clock.h"
#include "game/camera.h"
#include "game/screen.h"

bool SDL_STARTED = false;  // Define the global variable here.

Window::Window(const char* title, const int width, const int height, State* state) {

    // printf("Starting");
    if (!SDL_STARTED) {
        if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)){
            std::cout << "Failed to SDL: " << SDL_GetError() << std::endl;
        }
        if (!TTF_Init()) {
            std::cout << "Failed to TTF: " << SDL_GetError() << std::endl;
        }
        SDL_STARTED = true;
    }

    std::cout << "Window size " << width << "x" << height << "\n";

    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY); // 

    if(!window){
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
    }

    if (state) {
        this->state = state;
    } else {
        std::cout << "Creating state\n";
        this->state = new State();
    }
    this->state->input->setWindow(this);
    SDL_GetWindowSize(window, &this->state->screen->width, &this->state->screen->height);
    // SDL_GetWindowSize(window, &this->state->camera->width, &this->state->camera->height);
    this->state->camera->setSize(this->state->screen->width, this->state->screen->height);

    fullscreenCooldown = new Cooldown(200);
}

void Window::setScene(Scene* scene) {
    std::cout << "Setting scene renderer to state\n";
    state->renderer = scene->renderer;
    std::cout << "Setting scene to window\n";
    this->scene = scene;
    std::cout << "Resizing window to scene size\n";
    SDL_GetWindowSize(window, &scene->width, &scene->height);
    SDL_GetWindowSize(window, &this->state->screen->width, &this->state->screen->height);
    // SDL_GetWindowSize(window, &this->state->camera->width, &this->state->camera->height);
    this->state->camera->setSize(this->state->screen->width, this->state->screen->height);
}

void Window::onResize(int width, int height) {
    scene->width = width;
    scene->height = height;
    // state->camera->setWidth(width);
    // state->camera->setHeight(height);
    state->camera->setSize(width, height);
    state->screen->setSize(width, height);
    printf("Screen resize %ix%i\n", width, height);

}

bool Window::isFullscreen() {
    return fullscreen;
}

SDL_Window* Window::getInstance() {
    return window;
}

void Window::toggleFullscreen() {
    if (fullscreenCooldown->isReady()) {
        if (!window) return;

        if (!fullscreen) {
            printf("Going fullscreen\n");

            // Backup original size before switching to fullscreen
            SDL_GetWindowSize(window, &originalWidth, &originalHeight);

            const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(0);
            if (displayMode != nullptr) { // Get current monitor resolution
                SDL_SetWindowSize(window, displayMode->w, displayMode->h);  // Set window size to match screen
            }

            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
            fullscreen = true;
        } else {
            printf("Going normal screen\n");

            SDL_SetWindowFullscreen(window, 0);

            // Restore previous window size if available, otherwise use default 1280x720
            if (originalWidth > 0 && originalHeight > 0) {
                SDL_SetWindowSize(window, originalWidth, originalHeight);
            } else {
                SDL_SetWindowSize(window, 1280, 720);
            }

            fullscreen = false;
        }

        // Update state dimensions after toggling fullscreen
        SDL_GetWindowSize(window, &this->state->screen->width, &this->state->screen->height);
        this->state->camera->setSize(this->state->screen->width, this->state->screen->height);

        fullscreenCooldown->reset();
    }
}


int Window::run() {
    scene->prepare(state);

    bool running = true;

    while (!state->input->close) {
        static bool first = true;
        if (first) {
            printf("Window loop: tick\n");
        }
        state->clock->tick();
        if (first) {
            printf("Window loop: fetch\n");
        }
        state->input->fetch();
        if (first) {
            printf("Window loop: update\n");
        }
        scene->update(state);
        if (first) {
            printf("Window loop: render\n");
        }
        scene->render(state);
        if (first) {
            printf("Window loop: first frame done\n");
            first = false;
        }
    }

    return 0;
}
