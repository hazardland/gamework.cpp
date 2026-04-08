// File: image.cpp

#include "game/image.h"

#include <cmath>
#include <iostream>

Image::Image(SDL_Renderer* renderer, const char* path) {
    this->renderer = renderer;
    texture = IMG_LoadTexture(this->renderer, path);
    if (texture == nullptr) {
        std::cout << "Failed to load texture: " << path << " - " << SDL_GetError() << std::endl;
        width = 0;
        height = 0;
        return;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    float textureWidth = 0;
    float textureHeight = 0;
    SDL_GetTextureSize(texture, &textureWidth, &textureHeight);
    width = static_cast<int>(std::lround(textureWidth));
    height = static_cast<int>(std::lround(textureHeight));
}

Image::~Image() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
}

int Image::getWidth() {
    return width;
}

int Image::getHeight() {
    return height;
}

void Image::render(SDL_FRect* frame, SDL_FRect* position, SDL_FlipMode flip) {
    SDL_RenderTextureRotated(renderer, texture, frame, position, 0, NULL, flip);
}


