#ifndef KLAD1_ASSETS_H
#define KLAD1_ASSETS_H

class Sound;
class Sprite;

struct Sprites {
    Sprite* brick = nullptr;
    Sprite* bridge = nullptr;
    Sprite* player = nullptr;
    Sprite* ladder = nullptr;
    Sprite* gold = nullptr;
    Sprite* door = nullptr;
    Sprite* tide = nullptr;
    Sprite* key = nullptr;
    Sprite* bullet = nullptr;
};

struct Sounds {
    Sound* step = nullptr;
};

struct Assets {
    Sprites sprites;
    Sounds sounds;
};

#endif // KLAD1_ASSETS_H
