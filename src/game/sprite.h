// File: sprite.h

#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include <map>
#include <SDL3_image/SDL_image.h>

class Image;
class Frame;
class Clip;

/**
 * @brief The Sprite class encapsulates a sprite, which is a graphic that can have
 * multiple animations, represented by Clips. Each Clip is a set of frames of the Sprite.
 *
 * This class manages and renders these clips. It contains details about frame size,
 * pause between frames, reading direction and the actual image data. It also keeps a map
 * of different Clips.
 */
class Sprite {
public:
    // Frame width and height of each sprite frame
    int frameWidth;
    int frameHeight;

    // Time delay between frames
    int framePause;

    // Reading direction of the sprite sheet
    bool readVertically;

    // The actual image of the sprite
    Image* image;

    // Map of clip IDs to Clip objects
    std::map<int, Clip*> clips;

    // Functions declaration
    Sprite(Image* image, int frameWidth, int frameHeight, int framePause = 60, bool readVertically = false);
    Sprite* addClip(int clipId=1, int startCell=1, int startRow=1, int frameCount=1, bool flipX=false, bool flipY=false);
    Clip* getClip(int clipId);
    SDL_FRect* getRect(int clipId=1, int frameIndex=0);
    virtual ~Sprite();
};

#endif // GAME_SPRITE_H
