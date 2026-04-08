// File: scene.cpp

#include <iostream>
#include <cmath>

#include "game/scene.h"

#include "game/fps.h"
#include "game/debug.h"
#include "game/object.h"
#include "game/world.h"
#include "game/context.h"
#include "game/window.h"

Scene::Scene(Window* window, int width, int height) {

    this->window = window;
    int driverCount = SDL_GetNumRenderDrivers();
    std::cout << "Number of renderer drivers: " << driverCount << std::endl;

    // SDL_RendererInfo driverInfo;
    const char* preferedDriverName = "direct3d11";
    // int preferedDriverId = -1;
    // for(int i=0; i<driverCount; ++i)
    // {
    //     if(SDL_GetRenderDriverInfo(i, &driverInfo) == 0)
    //     {
    //         if (strcmp(driverInfo.name, preferedDriverName)==0) {
    //             preferedDriverId = i;
    //             std::cout << "[Driver match " << preferedDriverId << "]" << std::endl;
    //         }
    //         std::cout << "Driver " << i << ": " << driverInfo.name << std::endl;
    //     }
    // }

    renderer = SDL_CreateRenderer(window->getInstance(), preferedDriverName);
    // renderer = SDL_CreateRenderer(window, preferedDriverId, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    std::cout << "Chosen driver: " << SDL_GetRendererName(renderer) << std::endl;

    if(!renderer){
        std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window->getInstance());
        SDL_Quit();
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void Scene::setSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void Scene::prepare(Context* context) {
    // Empty in base class, can be overridden by derived classes
}

void Scene::update(Context* context) {
    debug(context, debugFont);

    if (client!=nullptr) {
        for (int i = 0; i < 10; ++i) {
            client->poll();
        }    
    }

    if (world!=nullptr) {
        world->update(context);
    }
    visibleObjects.clear();
    for (Object* object : objects)
    {
        object->update(context);
        if (object->isVisible(context)) {
            visibleObjects.push_back(object);
        }
    }
    if (client!=nullptr) {
        client->flush();
    }
}

void Scene::render(Context* context) {
    clear();
    build(context);
    present();
}

void Scene::clear() {
    SDL_RenderClear(renderer);
}

void Scene::build(Context* context) {
    if (world!=nullptr) {
       world->render(context);
    }
    for (Object* object : visibleObjects)
    {
        object->render(context);
    }

    // int tileSize = 8;
    // int lightRadius = 256;               // in tiles
    // int maxDistance = lightRadius * tileSize;
    
    // int coreRadius = 96;               // 🔥 Light fully visible up to this many pixels
    
    // int lightX = width / 2;
    // int lightY = height / 2;
    
    // for (int x = 0; x < width; x += tileSize) {
    //     for (int y = 0; y < height; y += tileSize) {
    //         int centerX = x + tileSize / 2;
    //         int centerY = y + tileSize / 2;
    
    //         float dx = centerX - lightX;
    //         float dy = centerY - lightY;
    //         float distance = std::sqrt(dx * dx + dy * dy);
    
    //         float fadedDistance = std::max(distance - coreRadius, 0.0f);  // starts fading after core
    //         float alpha = std::clamp((fadedDistance / (maxDistance - coreRadius)) * 255.0f, 0.0f, 255.0f);
    
    //         SDL_FRect rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(tileSize), static_cast<float>(tileSize) };
    //         SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(alpha));
    //         SDL_RenderFillRect(renderer, &rect);
    //     }
    // }
    
}

void Scene::present(int delay) {
    SDL_RenderPresent(renderer);
    // cout << "Presenting new frame\n";
    SDL_Delay(1);
}

void Scene::addObject(Object* obj) {
    objects.push_back(obj);
}

void Scene::addObject(Object* obj, uint32_t id) {
    objects.push_back(obj);
    objectById[id] = obj;
}

void Scene::addFps(TTF_Font* font) {
    if (fps != nullptr) {
        return;
    }

    fps = new Fps(font, "", 5, 5);
    fps->setColor({0, 255, 255, 255});
    addObject(fps);
}

void Scene::setDebugFont(TTF_Font* font) {
    debugFont = font;
}

Object* Scene::getObject(int id) {
    auto it = objectById.find(id);
    return it != objectById.end() ? it->second : nullptr;
}

void Scene::removeObject(Object* obj) {
    // Remove from map
    int id = obj->getId();
    objectById.erase(id);

    // Remove from vector (stable erase, order preserved)
    auto it = std::find(objects.begin(), objects.end(), obj);
    if (it != objects.end()) {
        objects.erase(it);
    }
}

void Scene::removeObject(int id) {
    auto it = objectById.find(id);
    if (it != objectById.end()) {
        Object* obj = it->second;
        objectById.erase(it);

        // Remove from vector
        auto vit = std::find(objects.begin(), objects.end(), obj);
        if (vit != objects.end()) {
            objects.erase(vit);
        }
    }
}


Scene::~Scene() {
    SDL_DestroyRenderer(renderer);
}



