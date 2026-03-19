// File: text.cpp

#include "game/text.h"
#include <string>


#include "game/object.h"
#include "game/state.h"
#include "game/camera.h"
#include "game/position.h"

Text::Text(TTF_Font* font, std::string text, int x, int y, SDL_Color color) {
    setPosition(x, y);
    this->color = color;
    this->font = font;
    positionFixed = true;
    // this->renderer = renderer;
}

Text::Text(TTF_Font* font, Position* position, std::string text, SDL_Color color) {
    this->position = position;
    this->color = color;
    this->font = font;
    // this->renderer = renderer;
    positionFixed = false;
}

Text* Text::setText(std::string text) {
    if (this->text==text){
        return this;
    }
    prepared = false;
    this->text = text;
    return this;
}

Text* Text::appendText(std::string text) {
    setText(this->text + text);
    return this;
}

Text* Text::setColor(SDL_Color color) {
    this->color.r = color.r;
    this->color.g = color.g;
    this->color.b = color.b;
    this->color.a = color.a;
    return this;
}

Text* Text::enableCache() {
    cacheEnabled = true;
    return this;
}

void Text::render(State* state, Position* position) {
    if (!visible || text.empty()) {
        return;
    }
    if (!prepared) {
        if (cacheEnabled && cache.contains(text)) {
            //printf("Cache hit %s\n", text);
            texture = cache[text];
            float width;
            float height;
            SDL_GetTextureSize(texture, &width, &height);
            position->setSize(width, height);
        } else {
            SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, {0,0,0,255});
            SDL_Surface* surfaceFg = TTF_RenderText_Blended(font, text.c_str(), 0, color);
            // Make sure both surfaces were created successfully
            if (surface && surfaceFg) {
                SDL_Rect srcRect = {0, 0, surfaceFg->w, surfaceFg->h};
                SDL_Rect destRect = {-1, -1, 0, 0}; // 1px offset for shadow
                SDL_BlitSurface(surfaceFg, &srcRect, surface, &destRect);
            }
            
            SDL_DestroySurface(surfaceFg); // correct way to free a surface
            if (surface==NULL) {
                printf("Failed to render text: %s", SDL_GetError());
            }
            position->setSize(surface->w, surface->h);
            if (texture!=nullptr) {
                if (!cacheEnabled) {
                    SDL_DestroyTexture(texture);
                }
            }
            texture = SDL_CreateTextureFromSurface(state->renderer, surface);
            if (cacheEnabled) {
                cache[text] = texture;
            }
            SDL_DestroySurface(surface);
        }
        prepared = true;
    }
    // printf("rendering tex");
    if (positionFixed) {
        SDL_RenderTexture(state->renderer, texture, NULL, position->getPosition());
    } else {
        SDL_RenderTexture(state->renderer, texture, NULL, state->camera->translate(position->getPosition()));
    }
}

void Text::render(State* state) {
    render(state, position);
}

Text* Text::setPositionFixed(bool value) {
    positionFixed = value;
    return this;
}

void Text::toggleVisible() {
    visible = !visible;
}


Text::~Text() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    // for (auto &item : cache) {
    //     SDL_DestroyTexture(item.second);
    // }
}
