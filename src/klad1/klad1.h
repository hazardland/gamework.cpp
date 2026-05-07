#ifndef KLAD1_SCENE_H
#define KLAD1_SCENE_H

#include <string>
#include <vector>

#include <SDL3_ttf/SDL_ttf.h>

#include "klad1/type.h"
#include "game/cooldown.h"
#include "game/scene.h"

class Image;
class Door;
class Exit;
class Gate;
class Gold;
class Key;
class Player;
class Enemy;
class Bullet;
class Sprite;
class Tide;
struct Level;

class Klad1 : public Scene {
    using Scene::Scene;

private:
    Image* image = nullptr;
    TTF_Font* font = nullptr;
    Player* player = nullptr;
    Exit* exit = nullptr;
    Key* key = nullptr;
    std::vector<Enemy*> enemies;
    std::vector<Door*> doors;
    std::vector<Gate*> gates;
    std::vector<Gold*> golds;
    std::vector<Bullet*> bullets;
    std::vector<Tide*> tides;
    float playerSpawnX = 0;
    float playerSpawnY = 0;
    int lives = 5;
    bool hasKey = false;
    bool bonusLifeGranted = false;
    int keyGold = -1;
    int goldCollected = 0;
    const Level* currentLevel = nullptr;
    int currentLevelIndex = 1;
    bool prevLevelSwitchLeft = false;
    bool prevLevelSwitchRight = false;
    bool prevDeleteDown = false;
    bool ioChordDown = false;
    Cooldown bulletCooldown = Cooldown(150);
    std::string windowTitle;
    bool levelRevealActive = false;
    int revealedRows = 0;
    Uint64 revealAccumulator = 0;
    bool debugMode = false;
    bool enemiesEnabled = true;

    void ensureAssets();
    void clearLevel();
    void loadLevel(const Level& level, bool resetLives);
    void reloadLevel();
    void spawnEnemies(const Level& level);
    void updateWindowTitle();
    void saveState() const;
    bool loadState();
    std::string getSavePath() const;
    void updateDebugMode();
    void setEnemiesEnabled(bool enabled);

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
    static constexpr int SPRITE_ENEMY_LIME = 10;
    static constexpr int SPRITE_ENEMY_PINK = 11;

    static constexpr int SOUND_STEP = 1;
    static constexpr int SOUND_GOLD = 2;
    static constexpr int SOUND_KEY = 3;
    static constexpr int SOUND_FIRE = 4;
    static constexpr int SOUND_HIT = 5;
    static constexpr int SOUND_DOOR = 6;
    static constexpr int SOUND_LEVEL = 7;

    static constexpr int LAYER_BACKGROUND = 0;
    static constexpr int LAYER_WORLD = 1;

    void prepare(Context* context) override;
    void update(Context* context) override;
    void render(Context* context) override;
    Player* getPlayer() const;
};

#endif // KLAD1_SCENE_H
