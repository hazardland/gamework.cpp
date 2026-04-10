#include "klad1/klad1.h"
#include "klad1/bridge.h"
#include "klad1/brick.h"
#include "klad1/door.h"
#include "klad1/enemy.h"
#include "klad1/exit.h"
#include "klad1/gate.h"
#include "klad1/gold.h"
#include "klad1/key.h"
#include "klad1/ladder.h"
#include "klad1/level.h"
#include "klad1/levels/level1.h"
#include "klad1/levels/level10.h"
#include "klad1/levels/level11.h"
#include "klad1/levels/level12.h"
#include "klad1/levels/level13.h"
#include "klad1/levels/level14.h"
#include "klad1/levels/level15.h"
#include "klad1/levels/level16.h"
#include "klad1/levels/level17.h"
#include "klad1/levels/level18.h"
#include "klad1/levels/level19.h"
#include "klad1/levels/level2.h"
#include "klad1/levels/level20.h"
#include "klad1/levels/level21.h"
#include "klad1/levels/level22.h"
#include "klad1/levels/level23.h"
#include "klad1/levels/level24.h"
#include "klad1/levels/level25.h"
#include "klad1/levels/level3.h"
#include "klad1/levels/level4.h"
#include "klad1/levels/level5.h"
#include "klad1/levels/level6.h"
#include "klad1/levels/level7.h"
#include "klad1/levels/level8.h"
#include "klad1/levels/level9.h"
#include "klad1/player.h"
#include "klad1/bullet.h"
#include "klad1/tide.h"
#include "klad1/unit_type.h"

#include "game/camera.h"
#include "game/cell.h"
#include "game/clip.h"
#include "game/clock.h"
#include "game/frame.h"
#include "game/image.h"
#include "game/input.h"
#include "game/screen.h"
#include "game/sound.h"
#include "game/world.h"
#include "game/sprite.h"
#include "game/context.h"
#include "game/window.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

namespace {
const Level* const LEVELS[] = {
    &level1, &level2, &level3, &level4, &level5,
    &level6, &level7, &level8, &level9, &level10,
    &level11, &level12, &level13, &level14, &level15,
    &level16, &level17, &level18, &level19, &level20,
    &level21, &level22, &level23, &level24, &level25
};

constexpr int LEVEL_COUNT = static_cast<int>(sizeof(LEVELS) / sizeof(LEVELS[0]));
constexpr const char* DEBUG_FILE = "debug.txt";
constexpr Uint64 LEVEL_REVEAL_ROW_MS = 70;
constexpr float DOOR_TOUCH_PADDING = 4.0f;

struct SavedPlayerState {
    bool hasValue = false;
    float x = 0;
    float y = 0;
    bool facingRight = true;
    float falling = 0;
};

struct SavedKeyState {
    bool exists = false;
    float x = 0;
    float y = 0;
};

struct SavedGoldState {
    int index = 0;
    bool collected = false;
};

struct SavedDoorState {
    float x = 0;
    float y = 0;
    bool open = false;
};

struct SavedEnemyState {
    int index = 0;
    float x = 0;
    float y = 0;
    bool facingRight = true;
    float falling = 0;
};

struct SavedBrickState {
    float x = 0;
    float y = 0;
    int damagedLeft = 0;
    int damagedRight = 0;
    bool regenerating = false;
    Uint64 regenElapsed = 0;
};

struct SavedBulletState {
    float x = 0;
    float y = 0;
    bool facingRight = true;
};
}

void Klad1::ensureAssets() {
    if (image != nullptr) {
        return;
    }

    image = new Image(renderer, "assets/klad1/sprites.png");
    font = TTF_OpenFont("assets/war2/fonts/titillium.ttf", 12);
    TTF_SetFontOutline(font, 1);
    setDebugFont(font);

    sounds[SOUND_STEP] = new Sound("assets/klad1/step.wav");
    sounds[SOUND_GOLD] = new Sound("assets/klad1/gold.wav");
    sounds[SOUND_KEY] = new Sound("assets/klad1/key.wav");
    sounds[SOUND_FIRE] = new Sound("assets/klad1/fire.wav");
    sounds[SOUND_HIT] = new Sound("assets/klad1/hit.wav");
    sounds[SOUND_DOOR] = new Sound("assets/klad1/door.wav");
    sounds[SOUND_LEVEL] = new Sound("assets/klad1/level.wav");

    sprites[SPRITE_BRICK] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))
        ->addClip(1, 8, 0, 1, false, false)
        ->addClip(2, 12, 0, 1, false, false)
        ->addClip(3, 13, 0, 1, false, false)
        ->addClip(4, 14, 0, 1, false, false)
        ->addClip(5, 15, 0, 1, false, false)
        ->addClip(6, 16, 0, 1, false, false)
        ->addClip(7, 17, 0, 1, false, false)
        ->addClip(8, 18, 0, 1, false, false)
        ->addClip(9, 19, 0, 1, false, false)
        ->addClip(10, 20, 0, 1, false, false)
        ->addClip(11, 46, 0, 1, false, false);
    sprites[SPRITE_BRIDGE] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 7, 0, 1, false, false);
    sprites[SPRITE_LADDER] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 6, 0, 1, false, false);
    sprites[SPRITE_GOLD] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 1, 0, 1, false, false);
    sprites[SPRITE_DOOR] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))
        ->addClip(1, 2, 0, 1, false, false)
        ->addClip(2, 11, 0, 1, false, false);
    sprites[SPRITE_TIDE] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 4, 0, 1, false, false);
    sprites[SPRITE_KEY] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))->addClip(1, 10, 0, 1, false, false);
    sprites[SPRITE_BULLET] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT))
        ->addClip(1, 31, 0, 1, false, false)
        ->addClip(2, 31, 0, 1, true, false);
    sprites[SPRITE_PLAYER] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT, 133))
        ->addClip(1, 24, 0, 1, false, false)
        ->addClip(2, 25, 0, 2, false, false)
        ->addClip(3, 26, 0, 4, false, false)
        ->addClip(4, 26, 0, 4, true, false)
        ->addClip(5, 26, 0, 1, true, false)
        ->addClip(6, 26, 0, 1, false, false);
    sprites[SPRITE_ENEMY_LIME] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT, 133))
        ->addClip(1, 32, 0, 1, false, false)
        ->addClip(2, 33, 0, 2, false, false)
        ->addClip(3, 34, 0, 4, false, false)
        ->addClip(4, 34, 0, 4, true, false)
        ->addClip(5, 34, 0, 1, true, false)
        ->addClip(6, 34, 0, 1, false, false);
    sprites[SPRITE_ENEMY_PINK] = (new Sprite(image, CELL_WIDTH, CELL_HEIGHT, 133))
        ->addClip(1, 39, 0, 1, false, false)
        ->addClip(2, 40, 0, 2, false, false)
        ->addClip(3, 41, 0, 4, false, false)
        ->addClip(4, 41, 0, 4, true, false)
        ->addClip(5, 41, 0, 1, true, false)
        ->addClip(6, 41, 0, 1, false, false);

    Clip* climbClip = sprites[SPRITE_PLAYER]->getClip(2);
    climbClip->frames[1].rect = climbClip->frames[0].rect;
    climbClip->frames[1].flipX = true;
    climbClip->frames[1].flipY = false;
    climbClip->frames[1].cacheFlip();

    climbClip = sprites[SPRITE_ENEMY_LIME]->getClip(2);
    climbClip->frames[1].rect = climbClip->frames[0].rect;
    climbClip->frames[1].flipX = true;
    climbClip->frames[1].flipY = false;
    climbClip->frames[1].cacheFlip();

    climbClip = sprites[SPRITE_ENEMY_PINK]->getClip(2);
    climbClip->frames[1].rect = climbClip->frames[0].rect;
    climbClip->frames[1].flipX = true;
    climbClip->frames[1].flipY = false;
    climbClip->frames[1].cacheFlip();
}

void Klad1::clearLevel() {
    clearObjects();
    fps = nullptr;

    if (world != nullptr) {
        delete world;
        world = nullptr;
    }

    player = nullptr;
    exit = nullptr;
    key = nullptr;
    doors.clear();
    gates.clear();
    golds.clear();
    enemies.clear();
    bullets.clear();
    tides.clear();
}

void Klad1::loadLevel(const Level& level, bool resetLives) {
    currentLevel = &level;
    clearLevel();
    levelRevealActive = false;
    revealedRows = Level::HEIGHT;
    revealAccumulator = 0;

    if (resetLives) {
        lives = 5;
    }

    hasKey = false;
    bonusLifeGranted = false;
    goldCollected = 0;
    keyGold = -1;
    bulletCooldown.reset();

    world = new World(
        image,
        CELL_WIDTH,
        CELL_HEIGHT,
        Level::WIDTH,
        Level::HEIGHT,
        2
    );

    if (!level.keyGolds.empty()) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> keyGoldDist(0, static_cast<int>(level.keyGolds.size()) - 1);
        keyGold = level.keyGolds[keyGoldDist(rng)];
    }

    std::vector<Position> tideSpawns;

    for (int y = 0; y < Level::HEIGHT; y++) {
        for (int x = 0; x < Level::WIDTH; x++) {
            uint8_t value = level.grid[y][x];
            world->setCell(x, y, TILE_BLANK, Level::BLANK);

            switch (value) {
                case Level::GOLD: {
                    Gold* gold = new Gold(x * CELL_WIDTH, y * CELL_HEIGHT, static_cast<int>(golds.size()) + 1);
                    addObject(gold);
                    golds.push_back(gold);
                    break;
                }

                case Level::DOOR: {
                    Door* door = new Door(x * CELL_WIDTH, y * CELL_HEIGHT);
                    door->setWorld(world);
                    addObject(door);
                    doors.push_back(door);
                    break;
                }

                case Level::EXIT: {
                    Gate* gate = new Gate(x * CELL_WIDTH, y * CELL_HEIGHT);
                    gate->setWorld(world);
                    addObject(gate);
                    gates.push_back(gate);
                    break;
                }

                case Level::TIDE: {
                    tideSpawns.push_back({
                        static_cast<uint8_t>(x),
                        static_cast<uint8_t>(y)
                    });
                    break;
                }

                case Level::WATER:
                    world->setCell(x, y, TILE_WATER, Level::WATER);
                    break;

                case Level::BRICK: {
                    Brick* brick = new Brick(x * CELL_WIDTH, y * CELL_HEIGHT);
                    brick->setWorld(world);
                    addObject(brick);
                    break;
                }

                case Level::LADDER: {
                    Ladder* ladder = new Ladder(x * CELL_WIDTH, y * CELL_HEIGHT);
                    ladder->setWorld(world);
                    addObject(ladder);
                    break;
                }

                case Level::BRIDGE: {
                    Bridge* bridge = new Bridge(x * CELL_WIDTH, y * CELL_HEIGHT);
                    bridge->setWorld(world);
                    addObject(bridge);
                    break;
                }

                case Level::WALL:
                    world->setCell(x, y, TILE_WALL, Level::WALL);
                    break;

                default:
                    break;
            }
        }
    }

    player = new Player(level.playerSpawn.x * CELL_WIDTH, level.playerSpawn.y * CELL_HEIGHT);
    player->setWorld(world);
    addObject(player);
    playerSpawnX = player->getX();
    playerSpawnY = player->getY();

    exit = new Exit(level.exit.x * CELL_WIDTH, level.exit.y * CELL_HEIGHT);
    addObject(exit);

    if (enemiesEnabled) {
        spawnEnemies(level);
    }

    for (const Position& tidePosition : tideSpawns) {
        Tide* tide = new Tide(tidePosition.x * CELL_WIDTH, tidePosition.y * CELL_HEIGHT);
        addObject(tide);
        tides.push_back(tide);
    }

    addFps(font);
    updateWindowTitle();
}

void Klad1::reloadLevel() {
    if (currentLevel != nullptr) {
        loadLevel(*currentLevel, false);
    }
}

void Klad1::spawnEnemies(const Level& level) {
    for (size_t i = 0; i < level.enemySpawns.size(); i++) {
        const Position& spawn = level.enemySpawns[i];
        if (spawn.x == 0 && spawn.y == 0) {
            continue;
        }

        int spriteId = (i % 2 == 0) ? SPRITE_ENEMY_LIME : SPRITE_ENEMY_PINK;
        Enemy* enemy = new Enemy(
            spawn.x * CELL_WIDTH,
            spawn.y * CELL_HEIGHT,
            spriteId
        );
        enemy->setWorld(world);
        addObject(enemy);
        enemies.push_back(enemy);
    }
}

std::string Klad1::getSavePath() const {
    return DEBUG_FILE;
}

void Klad1::updateDebugMode() {
    debugMode = std::filesystem::exists(DEBUG_FILE);
}

void Klad1::setEnemiesEnabled(bool enabled) {
    enemiesEnabled = enabled;

    if (enemiesEnabled) {
        if (currentLevel != nullptr && world != nullptr && enemies.empty()) {
            spawnEnemies(*currentLevel);
        }
        return;
    }

    for (Enemy* enemy : enemies) {
        if (enemy == nullptr) {
            continue;
        }
        removeObject(enemy);
        delete enemy;
    }

    enemies.clear();
}

void Klad1::updateWindowTitle() {
    if (window == nullptr || window->getInstance() == nullptr) {
        return;
    }

    std::ostringstream title;
    title << "Klad1 | Level " << (currentLevelIndex + 1)
          << " | Gold " << goldCollected << "/" << golds.size()
          << " | Lives " << lives;

    std::string nextTitle = title.str();
    if (nextTitle == windowTitle) {
        return;
    }

    windowTitle = nextTitle;
    SDL_SetWindowTitle(window->getInstance(), windowTitle.c_str());
}

void Klad1::saveState() const {
    std::ofstream out(getSavePath());
    if (!out) {
        return;
    }

    out << std::fixed << std::setprecision(3);
    out << "klad1_state 1\n";
    out << "level " << (currentLevelIndex + 1) << "\n";
    out << "lives " << lives << "\n";
    out << "hasKey " << (hasKey ? 1 : 0) << "\n";
    out << "bonusLifeGranted " << (bonusLifeGranted ? 1 : 0) << "\n";
    out << "enemiesEnabled " << (enemiesEnabled ? 1 : 0) << "\n";
    out << "keyGold " << keyGold << "\n";
    out << "goldCollected " << goldCollected << "\n";

    if (player != nullptr) {
        out << "player "
            << player->getX() << " "
            << player->getY() << " "
            << (player->isFacingRight() ? 1 : 0) << " "
            << player->getFalling() << "\n";
    }

    if (key != nullptr) {
        out << "key 1 " << key->getX() << " " << key->getY() << "\n";
    } else {
        out << "key 0\n";
    }

    for (Gold* gold : golds) {
        out << "gold "
            << gold->getIndex() << " "
            << gold->getX() << " "
            << gold->getY() << " "
            << (gold->isCollected() ? 1 : 0) << "\n";
    }

    for (Door* door : doors) {
        out << "door "
            << door->getX() << " "
            << door->getY() << " "
            << (door->isOpen() ? 1 : 0) << "\n";
    }

    for (Gate* gate : gates) {
        out << "gate "
            << gate->getX() << " "
            << gate->getY() << " "
            << (gate->isOpen() ? 1 : 0) << "\n";
    }

    for (size_t i = 0; i < enemies.size(); i++) {
        Enemy* enemy = enemies[i];
        out << "enemy "
            << i << " "
            << enemy->getX() << " "
            << enemy->getY() << " "
            << (enemy->isFacingRight() ? 1 : 0) << " "
            << enemy->getFalling() << "\n";
    }

    std::vector<const Brick*> writtenBricks;

    for (int y = 0; y < Level::HEIGHT; y++) {
        for (int x = 0; x < Level::WIDTH; x++) {
            if (world == nullptr) {
                break;
            }
            Cell* cell = world->grid[x][y];
            for (Unit* unit : cell->units[LAYER_WORLD]) {
                if (unit->getType() == UNIT_BRICK) {
                    Brick* brick = static_cast<Brick*>(unit);
                    if (std::find(writtenBricks.begin(), writtenBricks.end(), brick) != writtenBricks.end()) {
                        continue;
                    }
                    writtenBricks.push_back(brick);
                    out << "brick "
                        << brick->getX() << " "
                        << brick->getY() << " "
                        << brick->getDamagedLeft() << " "
                        << brick->getDamagedRight() << " "
                        << (brick->isRegenerating() ? 1 : 0) << " "
                        << brick->getRegenElapsed() << "\n";
                }
            }
        }
    }

    for (Bullet* bullet : bullets) {
        if (bullet == nullptr || bullet->isDead()) {
            continue;
        }
        out << "bullet "
            << bullet->getX() << " "
            << bullet->getY() << " "
            << (bullet->isFacingRight() ? 1 : 0) << "\n";
    }
}

bool Klad1::loadState() {
    std::ifstream in(getSavePath());
    if (!in) {
        return false;
    }

    int levelNumber = 1;
    int savedLives = lives;
    bool savedHasKey = false;
    bool savedBonusLifeGranted = false;
    bool savedEnemiesEnabled = true;
    int savedKeyGold = -1;
    int savedGoldCollected = 0;
    SavedPlayerState savedPlayer;
    SavedKeyState savedKey;
    std::vector<SavedGoldState> savedGolds;
    std::vector<SavedDoorState> savedDoors;
    std::vector<SavedDoorState> savedGates;
    std::vector<SavedEnemyState> savedEnemies;
    std::vector<SavedBrickState> savedBricks;
    std::vector<SavedBulletState> savedBullets;

    std::string kind;
    while (in >> kind) {
        if (kind == "klad1_state") {
            int version = 0;
            in >> version;
        } else if (kind == "level") {
            in >> levelNumber;
        } else if (kind == "lives") {
            in >> savedLives;
        } else if (kind == "hasKey") {
            int value = 0;
            in >> value;
            savedHasKey = value != 0;
        } else if (kind == "bonusLifeGranted") {
            int value = 0;
            in >> value;
            savedBonusLifeGranted = value != 0;
        } else if (kind == "enemiesEnabled") {
            int value = 0;
            in >> value;
            savedEnemiesEnabled = value != 0;
        } else if (kind == "keyGold") {
            in >> savedKeyGold;
        } else if (kind == "goldCollected") {
            in >> savedGoldCollected;
        } else if (kind == "player") {
            int facingRight = 0;
            in >> savedPlayer.x >> savedPlayer.y >> facingRight >> savedPlayer.falling;
            savedPlayer.hasValue = true;
            savedPlayer.facingRight = facingRight != 0;
        } else if (kind == "key") {
            int exists = 0;
            in >> exists;
            savedKey.exists = exists != 0;
            if (savedKey.exists) {
                in >> savedKey.x >> savedKey.y;
            }
        } else if (kind == "gold") {
            SavedGoldState state;
            int collected = 0;
            float x = 0;
            float y = 0;
            in >> state.index >> x >> y >> collected;
            state.collected = collected != 0;
            savedGolds.push_back(state);
        } else if (kind == "door") {
            SavedDoorState state;
            int open = 0;
            in >> state.x >> state.y >> open;
            state.open = open != 0;
            savedDoors.push_back(state);
        } else if (kind == "gate") {
            SavedDoorState state;
            int open = 0;
            in >> state.x >> state.y >> open;
            state.open = open != 0;
            savedGates.push_back(state);
        } else if (kind == "enemy") {
            SavedEnemyState state;
            int facingRight = 0;
            in >> state.index >> state.x >> state.y >> facingRight >> state.falling;
            state.facingRight = facingRight != 0;
            savedEnemies.push_back(state);
        } else if (kind == "brick") {
            SavedBrickState state;
            int regenerating = 0;
            in >> state.x >> state.y >> state.damagedLeft >> state.damagedRight >> regenerating >> state.regenElapsed;
            state.regenerating = regenerating != 0;
            savedBricks.push_back(state);
        } else if (kind == "bullet") {
            SavedBulletState state;
            int facingRight = 0;
            in >> state.x >> state.y >> facingRight;
            state.facingRight = facingRight != 0;
            savedBullets.push_back(state);
        } else {
            std::string rest;
            std::getline(in, rest);
        }
    }

    int targetLevelIndex = std::clamp(levelNumber - 1, 0, LEVEL_COUNT - 1);
    currentLevelIndex = targetLevelIndex;
    enemiesEnabled = savedEnemiesEnabled;
    loadLevel(*LEVELS[currentLevelIndex], false);

    lives = savedLives;
    hasKey = savedHasKey;
    bonusLifeGranted = savedBonusLifeGranted;
    keyGold = savedKeyGold;
    goldCollected = savedGoldCollected;

    if (savedPlayer.hasValue && player != nullptr) {
        player->restore(
            savedPlayer.x,
            savedPlayer.y,
            savedPlayer.facingRight,
            savedPlayer.falling,
            savedPlayer.falling > 0
        );
    }

    if (savedKey.exists) {
        key = new Key(savedKey.x, savedKey.y);
        addObject(key);
    }

    for (const SavedGoldState& state : savedGolds) {
        for (Gold* gold : golds) {
            if (gold->getIndex() == state.index) {
                gold->setCollected(state.collected);
                break;
            }
        }
    }

    for (const SavedDoorState& state : savedDoors) {
        for (Door* door : doors) {
            if (door->getX() == state.x && door->getY() == state.y) {
                door->setOpen(state.open);
                break;
            }
        }
    }

    for (const SavedDoorState& state : savedGates) {
        for (Gate* gate : gates) {
            if (gate->getX() == state.x && gate->getY() == state.y) {
                gate->setOpen(state.open);
                break;
            }
        }
    }

    for (const SavedEnemyState& state : savedEnemies) {
        if (state.index >= 0 && state.index < static_cast<int>(enemies.size())) {
            enemies[state.index]->restore(state.x, state.y, state.facingRight, state.falling);
        }
    }

    for (const SavedBrickState& state : savedBricks) {
        bool restored = false;
        for (int y = 0; y < Level::HEIGHT && !restored; y++) {
            for (int x = 0; x < Level::WIDTH && !restored; x++) {
                Cell* cell = world->grid[x][y];
                for (Unit* unit : cell->units[LAYER_WORLD]) {
                    if (unit->getType() != UNIT_BRICK) {
                        continue;
                    }

                    Brick* brick = static_cast<Brick*>(unit);
                    if (brick->getX() == state.x && brick->getY() == state.y) {
                        brick->setState(
                            state.damagedLeft,
                            state.damagedRight,
                            state.regenerating,
                            state.regenElapsed
                        );
                        restored = true;
                        break;
                    }
                }
            }
        }
    }

    for (const SavedBulletState& state : savedBullets) {
        Bullet* bullet = new Bullet(state.x, state.y, state.facingRight);
        bullet->setWorld(world);
        addObject(bullet);
        bullets.push_back(bullet);
    }

    updateWindowTitle();

    return true;
}

void Klad1::prepare(Context* context) {
    Scene::prepare(context);
    updateDebugMode();
    ensureAssets();
    currentLevelIndex = 0;
    loadLevel(*LEVELS[currentLevelIndex], true);
    Scene::update(context);
    sounds[SOUND_LEVEL]->play(context);
    levelRevealActive = true;
    revealedRows = 0;
    revealAccumulator = 0;
    context->camera->setZoom(1.5f);
}

void Klad1::update(Context* context) {
    updateDebugMode();

    Keyboard* keyboard = context->input->keyboard;
    bool ctrl = keyboard->keys != nullptr &&
        (keyboard->keys[SDL_SCANCODE_LCTRL] || keyboard->keys[SDL_SCANCODE_RCTRL]);
    bool saveNow = debugMode && ctrl && keyboard->keys[SDL_SCANCODE_S];
    bool loadNow = debugMode && ctrl && keyboard->keys[SDL_SCANCODE_O];

    if (saveNow || loadNow) {
        if (!ioChordDown) {
            if (saveNow) {
                saveState();
            } else {
                loadState();
            }
        }
        ioChordDown = true;
        return;
    }

    ioChordDown = false;

    bool deleteDown = debugMode && keyboard->keys[SDL_SCANCODE_DELETE];
    bool deletePressed = deleteDown && !prevDeleteDown;
    prevDeleteDown = deleteDown;

    if (deletePressed) {
        setEnemiesEnabled(!enemiesEnabled);
        return;
    }

    bool switchLeft = debugMode && ctrl && keyboard->left;
    bool switchRight = debugMode && ctrl && keyboard->right;
    bool switchLeftPressed = switchLeft && !prevLevelSwitchLeft;
    bool switchRightPressed = switchRight && !prevLevelSwitchRight;

    if (levelRevealActive) {
        revealAccumulator += context->clock->delta;
        while (revealAccumulator >= LEVEL_REVEAL_ROW_MS) {
            revealAccumulator -= LEVEL_REVEAL_ROW_MS;
            revealedRows++;
            if (revealedRows >= Level::HEIGHT) {
                revealedRows = Level::HEIGHT;
                levelRevealActive = false;
                break;
            }
        }
        return;
    }

    Scene::update(context);

    if (player == nullptr) {
        return;
    }

    bool shouldReloadLevel = false;
    bool shouldAdvanceLevel = false;

    if (switchLeftPressed || switchRightPressed) {
        if (switchRightPressed) {
            currentLevelIndex = (currentLevelIndex + 1) % LEVEL_COUNT;
        } else {
            currentLevelIndex = (currentLevelIndex - 1 + LEVEL_COUNT) % LEVEL_COUNT;
        }
        loadLevel(*LEVELS[currentLevelIndex], true);
        prevLevelSwitchLeft = switchLeft;
        prevLevelSwitchRight = switchRight;
        return;
    }

    prevLevelSwitchLeft = switchLeft;
    prevLevelSwitchRight = switchRight;

    if (keyboard->space && bulletCooldown.isReady()) {
        Bullet* bullet = new Bullet(player->getBulletX(), player->getBulletY(), player->isFacingRight());
        bullet->setWorld(world);
        addObject(bullet);
        bullets.push_back(bullet);
        bulletCooldown.reset();
        player->shoot();
        sounds[SOUND_FIRE]->play(context);
    }

    for (Tide* tide : tides) {
        if (tide != nullptr && player->intersects(tide)) {
            if (lives > 0) {
                lives--;
            }
            shouldReloadLevel = true;
            break;
        }

        for (Enemy* enemy : enemies) {
            if (enemy != nullptr && enemy->intersects(tide)) {
                enemy->respawn();
            }
        }
    }

    for (Enemy* enemy : enemies) {
        if (enemy != nullptr && player->intersects(enemy)) {
            if (lives > 0) {
                lives--;
            }
            shouldReloadLevel = true;
            break;
        }
    }

    if (shouldReloadLevel) {
        if (lives <= 0) {
            currentLevelIndex = 0;
            loadLevel(*LEVELS[currentLevelIndex], true);
        } else {
            reloadLevel();
        }
        return;
    }

    if (exit != nullptr && player->intersects(exit)) {
        shouldAdvanceLevel = true;
    }

    for (auto it = bullets.begin(); it != bullets.end();) {
        Bullet* bullet = *it;
        if (bullet == nullptr || !bullet->isDead()) {
            ++it;
            continue;
        }

        removeObject(bullet);
        delete bullet;
        it = bullets.erase(it);
    }

    bool spawnedKeyThisFrame = false;

    for (Gold* gold : golds) {
        if (gold == nullptr || gold->isCollected()) {
            continue;
        }

        if (!player->intersects(gold)) {
            continue;
        }

        gold->collect();
        goldCollected++;
        sounds[SOUND_GOLD]->play(context);
        updateWindowTitle();

        if (gold->getIndex() == keyGold && key == nullptr) {
            key = new Key(gold->getX(), gold->getY());
            addObject(key);
            spawnedKeyThisFrame = true;
            sounds[SOUND_KEY]->play(context);
        }
    }

    if (currentLevel != nullptr &&
        currentLevel->hasBonusLife &&
        !bonusLifeGranted &&
        goldCollected == static_cast<int>(golds.size())) {
        if (lives < 5) {
            lives++;
            updateWindowTitle();
        }
        bonusLifeGranted = true;
    }

    if (!spawnedKeyThisFrame && key != nullptr && !hasKey && player->intersects(key)) {
        hasKey = true;
    }

    for (Door* door : doors) {
        if (door != nullptr && !door->isOpen() &&
            player->intersects(door, DOOR_TOUCH_PADDING, 1, DOOR_TOUCH_PADDING, 1)) {
            door->open();
            sounds[SOUND_DOOR]->play(context);
        }
    }

    if (hasKey) {
        for (Gate* gate : gates) {
            if (gate != nullptr && !gate->isOpen() &&
                player->intersects(gate, DOOR_TOUCH_PADDING, 1, DOOR_TOUCH_PADDING, 1)) {
                gate->open();
                sounds[SOUND_DOOR]->play(context);
            }
        }
    }

    if (shouldAdvanceLevel) {
        if (currentLevelIndex < LEVEL_COUNT - 1) {
            sounds[SOUND_LEVEL]->play(context);
            currentLevelIndex++;
            loadLevel(*LEVELS[currentLevelIndex], false);
            Scene::update(context);
            levelRevealActive = true;
            revealedRows = 0;
            revealAccumulator = 0;
        }
        return;
    }
}

void Klad1::render(Context* context) {
    Scene::clear();
    Scene::build(context);

    if (levelRevealActive) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int row = revealedRows; row < Level::HEIGHT; row++) {
            SDL_FRect worldRect = {
                0.0f,
                static_cast<float>(row * CELL_HEIGHT),
                static_cast<float>(Level::WIDTH * CELL_WIDTH),
                static_cast<float>(CELL_HEIGHT)
            };
            SDL_FRect* screenRect = context->camera->translate(&worldRect);
            SDL_RenderFillRect(renderer, screenRect);
        }
    }

    Scene::present();
}

Player* Klad1::getPlayer() const {
    return player;
}
