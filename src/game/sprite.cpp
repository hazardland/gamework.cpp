#include "game/sprite.h"

#include <map>
#include <SDL3_image/SDL_image.h>

#include "game/clip.h"
#include "game/image.h"
#include "game/frame.h"

/**
 * @brief The Sprite class encapsulates a sprite, which is a graphic that can have
 * multiple animations, represented by Clips. Each Clip is a set of frames of the Sprite.
 *
 * This class manages and renders these clips. It contains details about frame size,
 * pause between frames, reading direction and the actual image data. It also keeps a map
 * of different Clips.
 */

Sprite::Sprite (Image* image,
            int frameWidth,
            int frameHeight,
            int framePause,
            bool readVertically
            )
{
        this->frameWidth = frameWidth;
        this->frameHeight = frameHeight;
        this->framePause = framePause;
        this->image = image;
        this->readVertically = readVertically;
}

// addClip method
Sprite* Sprite::addClip(int clipId,
                        int startCell,
                        int startRow, 
                        int frameCount,
                        bool flipX,
                        bool flipY) {
    clips[clipId] = new Clip(image,
                             frameWidth,
                             frameHeight,
                             startCell, startRow,
                             frameCount,
                             framePause,
                             flipX, flipY, readVertically);
    return this;
}

Clip* Sprite::getClip(int clipId) {
    return clips[clipId];
}

// getRect method
SDL_FRect* Sprite::getRect(int clipId, int frameIndex) {
    return getClip(clipId)->getFrame(frameIndex)->getRect();
}

// Destructor
Sprite::~Sprite() {
    delete image;
}
