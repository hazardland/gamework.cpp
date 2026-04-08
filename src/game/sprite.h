#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include <map>
#include <SDL3_image/SDL_image.h>

class Image;
class Clip;

class Sprite {
public:
    int frameWidth;
    int frameHeight;
    int framePause;
    bool readVertically;
    Image* image;
    std::map<int, Clip*> clips;

    Sprite(Image* image, int frameWidth, int frameHeight, int framePause = 60, bool readVertically = false);
    Sprite* addClip(int clipId=1, int startCell=0, int startRow=0, int frameCount=1, bool flipX=false, bool flipY=false);
    Clip* getClip(int clipId);
    SDL_FRect* getRect(int clipId=1, int frameIndex=0);
    virtual ~Sprite();
};

#endif // GAME_SPRITE_H


