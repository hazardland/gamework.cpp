#ifndef GAME_IMAGE_H
#define GAME_IMAGE_H

#include <SDL3_image/SDL_image.h>

class Image {
public:
    Image(SDL_Renderer* renderer, const char* path);
    ~Image();

    int getWidth();
    int getHeight();
    void render(SDL_FRect* frame, SDL_FRect* position, SDL_FlipMode flip = SDL_FLIP_NONE);

    SDL_Renderer* renderer;
private:
    SDL_Texture* texture = nullptr;
    int width;
    int height;
};

#endif // GAME_IMAGE_H


