#include <iostream>

#include "game/window.h"

#include "game/scene.h"
#include "game/audio.h"
#include "game/context.h"
#include "game/input.h"
#include "game/clock.h"
#include "game/camera.h"
#include "game/screen.h"

bool SDL_STARTED = false;

Window::Window(const char* title, int width, int height, Context* context) {
    if (!SDL_STARTED) {
        if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)){
            std::cout << "Failed to SDL: " << SDL_GetError() << std::endl;
        }
        if (!TTF_Init()) {
            std::cout << "Failed to TTF: " << SDL_GetError() << std::endl;
        }
        SDL_STARTED = true;
    }

    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if(!window){
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
    }

    if (context) {
        this->context = context;
    } else {
        this->context = new Context();
    }

    this->context->input->setWindow(this);
    SDL_GetWindowSize(window, &this->context->screen->width, &this->context->screen->height);
    this->context->camera->setSize(this->context->screen->width, this->context->screen->height);

    fullscreenCooldown = new Cooldown(200);
}

void Window::setScene(Scene* scene) {
    context->renderer = scene->renderer;
    this->scene = scene;
    SDL_GetWindowSize(window, &scene->width, &scene->height);
    SDL_GetWindowSize(window, &this->context->screen->width, &this->context->screen->height);
    this->context->camera->setSize(this->context->screen->width, this->context->screen->height);
}

void Window::onResize(int width, int height) {
    scene->width = width;
    scene->height = height;
<<<<<<< HEAD
    // state->camera->setWidth(width);
    // state->camera->setHeight(height);
    state->camera->setSize(width, height);
    state->screen->setSize(width, height);
    printf("Screen resize %ix%i\n", width, height);
=======
    context->camera->setSize(width, height);
    context->screen->setSize(width, height);
>>>>>>> klad1
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
            SDL_GetWindowSize(window, &originalWidth, &originalHeight);

            const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(0);
            if (displayMode != nullptr) {
                SDL_SetWindowSize(window, displayMode->w, displayMode->h);
            }

            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
            fullscreen = true;
        } else {
            SDL_SetWindowFullscreen(window, 0);

            if (originalWidth > 0 && originalHeight > 0) {
                SDL_SetWindowSize(window, originalWidth, originalHeight);
            } else {
                SDL_SetWindowSize(window, 1280, 720);
            }

            fullscreen = false;
        }

        SDL_GetWindowSize(window, &this->context->screen->width, &this->context->screen->height);
        this->context->camera->setSize(this->context->screen->width, this->context->screen->height);

        fullscreenCooldown->reset();
    }
}


int Window::run() {
    scene->prepare(context);

    while (!context->input->close) {
        context->clock->tick();
        context->input->poll();
        scene->update(context);
        context->audio->update();
        scene->render(context);
    }

    return 0;
}

Window::~Window() {
    delete fullscreenCooldown;
}



