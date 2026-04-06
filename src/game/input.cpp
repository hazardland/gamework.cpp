#include "game/input.h"
class Window;

void Input::setWindow(Window* window) {
    this->window = window;
}

void Input::fetch() {
    static bool first = true;
    if (first) {
        printf("Input::fetch start\n");
    }
    keyboard->reset();
    mouse->reset();

    //SDL_SetWindowTitle(window, to_string(clock.avgFps).c_str());
    if (first) {
        printf("Input::fetch before mouse state\n");
    }
    SDL_GetMouseState(&mouse->x, &mouse->y);
    if (first) {
        printf("Input::fetch after mouse state\n");
    }

        // If drag ended in previous tick we reset drag
        if (mouse->rightDragEnded) {
            mouse->rightDragEnded = false;
            mouse->rightDragStartX = -1;
            mouse->rightDragStartY = -1;
            // cout << "Cleaning right drag\n";
        // So basically if mause was down startX becomes>-1
        // If startX != x means we moved
        // So it can be treated as drag start
        } else if (!mouse->rightDragActive
                   && mouse->rightDragStartX>-1
                   && (mouse->rightDragStartX!=mouse->x
                       || mouse->rightDragStartY!=mouse->y)) {
            mouse->rightDragActive = true;
            // cout << "Activating right drag\n";
        }

        if (mouse->leftDragEnded) {
            mouse->leftDragEnded = false;
            mouse->leftDragStartX = -1;
            mouse->leftDragStartY = -1;
            // cout << "Cleaning left drag\n";
        } else if (!mouse->leftDragActive
                   && mouse->leftDragStartX>-1
                   && (mouse->leftDragStartX!=mouse->x
                       || mouse->leftDragStartY!=mouse->y)) {
            mouse->leftDragActive = true;
            // cout << "Activating left drag\n";
        }

        SDL_Event event;
        if (first) {
            printf("Input::fetch before poll\n");
        }
        while(SDL_PollEvent(&event)){
            if (first) {
                printf("Input::fetch event type=%u\n", event.type);
            }

            switch(event.type){
                case SDL_EVENT_QUIT:
                    close = true;
                    break;
                // case SDL_WINDOWEVENT:
                    // switch (event.window.event){
                        case SDL_EVENT_WINDOW_RESIZED:
                            // window->width = event.window.data1;
                            // window->height = event.window.data2;
                            window->onResize(event.window.data1, event.window.data2);
                            //SDL_Log("Window %d resized to %dx%d",
                            //        event.window.windowID, event.window.data1, event.window.data2);
                        break;
                    // }
                    // break;
                // case SDL_KEYDOWN:
                //     keyboard->keys = SDL_GetKeyboardState(nullptr);
                //     cout << "some key is down\n";
                //     if (keyboard->keys[SDL_SCANCODE_UP]) {
                //         cout << ">>>>> up is pressed\n";
                //         keyboard->up = true;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_DOWN]) {
                //         keyboard->down = true;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_RIGHT]) {
                //         keyboard->right = true;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_LEFT]) {
                //         keyboard->left = true;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_SPACE]) {
                //         keyboard->space = true;
                //     }
                //     break;
                // case SDL_KEYUP:
                //     cout << "some key is up\n";
                //     keyboard->keys = SDL_GetKeyboardState(nullptr);
                //     if (keyboard->keys[SDL_SCANCODE_UP]) {
                //         cout << "<<<<< up is released\n";
                //         keyboard->up = false;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_DOWN]) {
                //         keyboard->down = false;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_RIGHT]) {
                //         keyboard->right = false;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_LEFT]) {
                //         keyboard->left = false;
                //     }
                //     if (keyboard->keys[SDL_SCANCODE_SPACE]) {
                //         keyboard->space = false;
                //     }
                //     break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    // This could be potential drag start so save x, y
                    // Drag will only be activated on next tick
                    // If mouse moves to another position
                    // But before mouse is UP
                    if (event.button.button==SDL_BUTTON_RIGHT) {
                        mouse->rightDragStartX = mouse->x;
                        mouse->rightDragStartY = mouse->y;
                        // cout << "Right mouse down\n";
                    } else if (event.button.button==SDL_BUTTON_LEFT) {
                        mouse->leftDragStartX = mouse->x;
                        mouse->leftDragStartY = mouse->y;
                        // cout << "Left mouse down\n";
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button==SDL_BUTTON_RIGHT) {
                        // So if ever drag activated
                        // We can deactivate drag
                        if (mouse->rightDragActive) {
                            mouse->rightDragEnded = true;
                            mouse->rightDragActive = false;
                            // cout << "Right drag end\n";
                        // So if drag is not active
                        } else {
                            mouse->rightClick = true;
                            mouse->rightDragStartX = -1;
                            mouse->rightDragStartY = -1;
                            // cout << "Right click\n";
                        }
                    // I believe this event come separately
                    } else if (event.button.button==SDL_BUTTON_LEFT) {
                        if (mouse->leftDragActive) {
                            mouse->leftDragEnded = true;
                            mouse->leftDragActive = false;
                            // cout << "Left drag end\n";
                        } else {
                            mouse->leftClick = true;
                            mouse->leftDragStartX = -1;
                            mouse->leftDragStartY = -1;
                            // cout << "Left click\n";
                        }
                    }
                    // if any of drags active we just ended drag

                    break;

                default:
                    break;
            }

        }
    if (first) {
        printf("Input::fetch after poll\n");
    }

    // update keyboard state here
    keyboard->keys = SDL_GetKeyboardState(nullptr);
    if (keyboard->keys[SDL_SCANCODE_UP]) {
        keyboard->up = true;
    } else {
        keyboard->up = false;
    }
    if (keyboard->keys[SDL_SCANCODE_DOWN]) {
        keyboard->down = true;
    } else {
        keyboard->down = false;
    }
    if (keyboard->keys[SDL_SCANCODE_RIGHT]) {
        keyboard->right = true;
    } else {
        keyboard->right = false;
    }
    if (keyboard->keys[SDL_SCANCODE_LEFT]) {
        keyboard->left = true;
    } else {
        keyboard->left = false;
    }
    if (keyboard->keys[SDL_SCANCODE_SPACE]) {
        keyboard->space = true;
    } else {
        keyboard->space = false;
    }
    if (keyboard->keys[SDL_SCANCODE_EQUALS]) {
        keyboard->plus = true;
    } else {
        keyboard->plus = false;
    }
    if (keyboard->keys[SDL_SCANCODE_MINUS]) {
        keyboard->minus = true;
    } else {
        keyboard->minus = false;
    }
    if (keyboard->keys[SDL_SCANCODE_LALT] || keyboard->keys[SDL_SCANCODE_RALT]) {
        keyboard->alt = true;
    } else {
        keyboard->alt = false;
    }
    if (keyboard->keys[SDL_SCANCODE_RETURN]) {
        keyboard->enter = true;
    } else {
        keyboard->enter = false;
    }
    if (keyboard->keys[SDL_SCANCODE_A]) {
        keyboard->a = true;
    } else {
        keyboard->a = false;
    }
    if (keyboard->keys[SDL_SCANCODE_W]) {
        keyboard->w = true;
    } else {
        keyboard->w = false;
    }
    if (keyboard->keys[SDL_SCANCODE_S]) {
        keyboard->s = true;
    } else {
        keyboard->s = false;
    }
    if (keyboard->keys[SDL_SCANCODE_D]) {
        keyboard->d = true;
    } else {
        keyboard->d = false;
    }
    if (keyboard->keys[SDL_SCANCODE_F5]) {
        keyboard->f5 = true;
    } else {
        keyboard->f5 = false;
    }
    if (keyboard->keys[SDL_SCANCODE_F11]) {
        keyboard->f11 = true;
    } else {
        keyboard->f11 = false;
    }
    if (keyboard->keys[SDL_SCANCODE_F12]) {
        keyboard->f12 = true;
    } else {
        keyboard->f12 = false;
    }
    if (keyboard->keys[SDL_SCANCODE_TAB]) {
        keyboard->tab = true;
    } else {
        keyboard->tab = false;
    }

    if (first) {
        printf("Input::fetch done\n");
        first = false;
    }

}

Input::~Input() {
    delete keyboard;
    delete mouse;
}
