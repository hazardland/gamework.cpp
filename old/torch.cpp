#include "game/torch.h"
#include "game/context.h"
#include "game/screen.h"
#include <cmath>
#include <algorithm>

Torch::Torch(SDL_Color color, int coreRadius, int lightRadius)
    : color(color),
      coreRadius(coreRadius), lightRadius(lightRadius),
      visible(true) {}

void Torch::update(Context* context) {


}

void Torch::render(Context* context) {
    return;
    if (!visible) return;

    SDL_Renderer* renderer = context->renderer;
    int screenWidth = context->screen->getWidth();
    int screenHeight = context->screen->getHeight();
    int tileSize = 16;

    int lightX = screenWidth / 2;
    int lightY = screenHeight / 2;
    int maxDistance = lightRadius;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Only loop around a square bounding box around the torch radius
    int minX = std::max(0, lightX - lightRadius);
    int maxX = std::min(screenWidth, lightX + lightRadius);
    int minY = std::max(0, lightY - lightRadius);
    int maxY = std::min(screenHeight, lightY + lightRadius);


    for (int x = minX; x < maxX; x += tileSize) {
        for (int y = minY; y < maxY; y += tileSize) {
            int centerX = x + tileSize / 2;
            int centerY = y + tileSize / 2;

            float dx = static_cast<float>(centerX - lightX);
            float dy = static_cast<float>(centerY - lightY);
            float distance = std::sqrt(dx * dx + dy * dy);

            // Skip pixels completely outside the light radius
            if (distance > lightRadius) continue;

            float fadedDistance = std::max(distance - static_cast<float>(coreRadius), 0.0f);
            float alpha = std::clamp((fadedDistance / (maxDistance - coreRadius)) * 255.0f, 0.0f, 255.0f);

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, static_cast<Uint8>(alpha));
            SDL_FRect rect = {
                static_cast<float>(x),
                static_cast<float>(y),
                static_cast<float>(tileSize),
                static_cast<float>(tileSize)
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}


