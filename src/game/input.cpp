#include "game/input.h"
#include "game/window.h"

void Input::setWindow(Window* window) {
    this->window = window;
}

void Input::poll() {
    mouse->reset();

    float x = 0;
    float y = 0;
    SDL_GetMouseState(&x, &y);
    mouse->move(x, y);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        handle(event);
    }

    keyboard->read(SDL_GetKeyboardState(nullptr));
}

void Input::handle(SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_QUIT:
            close = true;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            if (window != nullptr) {
                window->onResize(event.window.data1, event.window.data2);
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouse->down(event.button.button);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouse->up(event.button.button);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            mouse->move(event.motion.x, event.motion.y);
            break;

        default:
            break;
    }
}

Input::~Input() {
    delete keyboard;
    delete mouse;
}


