#ifndef GAME_SOUND_H
#define GAME_SOUND_H

#include <SDL3/SDL.h>

class Context;
class Audio;

class Sound {
    friend class Audio;

public:
    Sound(const char* path);
    ~Sound();

    void play(Context* context) const;
    bool isReady() const;

private:
    SDL_AudioSpec spec = {};
    Uint8* buffer = nullptr;
    Uint32 length = 0;
};

#endif // GAME_SOUND_H
