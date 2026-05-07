#ifndef GAME_ANIMATION_H
#define GAME_ANIMATION_H

#include <SDL3_image/SDL_image.h>

class Clip;
class Sprite;

class Animation {
    public:
    int pause = 0;
    int timer = 0;
    int frame = 0;

    Sprite* sprite;
    Clip* clip;
    int activeClip = 0;

    Animation(Sprite* sprite, int defaultClipName);
    virtual ~Animation() = default;
    virtual void render(SDL_FRect* position);
    virtual void play(int clipName, float speed=0);
    virtual void update(uint32_t delta);
};

#endif


