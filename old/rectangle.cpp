// File: rectangle.cpp

#include "game/rectangle.h"

#include "game/context.h"

Rectangle::Rectangle(SDL_Color border, SDL_Color background)
: border(border), background(background), visible(true) {}

void Rectangle::show() {
    visible = true;
}

void Rectangle::hide() {
    visible = false;
}

void Rectangle::render(Context* context) {
    if (visible) {
        if (background.a>0) {
            SDL_SetRenderDrawColor(context->renderer, background.r, background.g, background.b, background.a);
            SDL_RenderRect(context->renderer, getPosition());
        }

        if (border.a>0){
            SDL_SetRenderDrawColor(context->renderer, border.r, border.g, border.b, border.a);
            SDL_RenderRect(context->renderer, getPosition());
        }

        if (background.a>0 || border.a>0){
            SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 0);
        }
    }
}


