#ifndef GAME_CLIP_H
#define GAME_CLIP_H

class Frame;
class Image;

class Clip {
public:
    Frame* frames;
    int frameCount;
    int framePause;
    Clip(Image* image, int frameWidth, int frameHeight, int startCell, int startRow, int frameCount=1, int framePause=60, bool flipX=false, bool flipY=false, bool readVertically=false);
    int getFrameCount();
    Frame* getFrame(int frameIndex);
    int getFramePause();
    virtual ~Clip();
};

#endif // GAME_CLIP_H


