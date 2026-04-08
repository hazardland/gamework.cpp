// File: clip.cpp

#include "game/clip.h"

#include "game/frame.h"
#include "game/image.h"

Clip::Clip(Image* image, int frameWidth, int frameHeight, int startCell, int startRow, int frameCount, int framePause, bool flipX, bool flipY, bool readVertically) {
    this->framePause = framePause;
    this->frameCount = frameCount;
    frames = new Frame[frameCount];
    int cellCount = image->getWidth() / frameWidth;
    int rowCount = image->getHeight() / frameHeight;
    int cell = startCell;
    int row = startRow;

    // Create frames
    for (int i = 0; i < frameCount; i++) {
        frames[i].rect.x = cell * frameWidth;
        frames[i].rect.y = row * frameHeight;
        frames[i].rect.w = frameWidth;
        frames[i].rect.h = frameHeight;
        frames[i].flipX = flipX;
        frames[i].flipY = flipY;
        frames[i].cacheFlip();

        // Update cell and row for next frame
        if (readVertically) {
            if (row == rowCount - 1) {
                row = 0;
                cell += 1;
            } else {
                row += 1;
            }
        } else {
            if (cell == cellCount - 1) {
                cell = 0;
                row += 1;
            } else {
                cell += 1;
            }
        }
    }
}

int Clip::getFrameCount() {
    return frameCount;
}

Frame* Clip::getFrame(int frameIndex) {
    return &frames[frameIndex];
}

int Clip::getFramePause() {
    return framePause;
}

Clip::~Clip() {
    delete[] frames;
}


