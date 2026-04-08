#ifndef KLAD1_SCENE_H
#define KLAD1_SCENE_H

#include <vector>

#include <SDL3_ttf/SDL_ttf.h>

#include "klad1/type.h"
#include "game/cooldown.h"
#include "game/scene.h"

class Image;
class Gold;
class Key;
class Player;
class Bullet;
class Sprite;
class Tide;

class Klad1 : public Scene {
    using Scene::Scene;

private:
    Image* image = nullptr;
    TTF_Font* font = nullptr;
    Player* player = nullptr;
    Key* key = nullptr;
    std::vector<Gold*> golds;
    std::vector<Bullet*> bullets;
    std::vector<Tide*> tides;
    float playerSpawnX = 0;
    float playerSpawnY = 0;
    int lives = 5;
    int keyGold = -1;
    int goldCollected = 0;
    Cooldown bulletCooldown = Cooldown(300);

public:
    static constexpr int CELL_WIDTH = 32;
    static constexpr int CELL_HEIGHT = 22;

    static constexpr int SPRITE_BRICK = 1;
    static constexpr int SPRITE_BRIDGE = 2;
    static constexpr int SPRITE_PLAYER = 3;
    static constexpr int SPRITE_LADDER = 4;
    static constexpr int SPRITE_GOLD = 5;
    static constexpr int SPRITE_DOOR = 6;
    static constexpr int SPRITE_TIDE = 7;
    static constexpr int SPRITE_KEY = 8;
    static constexpr int SPRITE_BULLET = 9;

    static constexpr int LAYER_BACKGROUND = 0;
    static constexpr int LAYER_WORLD = 1;

    void prepare(Context* context) override;
    void update(Context* context) override;
};

#endif // KLAD1_SCENE_H


