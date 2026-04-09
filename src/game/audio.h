#ifndef GAME_AUDIO_H
#define GAME_AUDIO_H

#include <vector>

#include <SDL3/SDL.h>

class Sound;

class Audio {
public:
    Audio();
    ~Audio();

    void play(const Sound* sound);
    void update();
    bool isReady() const;

private:
    SDL_AudioDeviceID device = 0;
    SDL_AudioSpec deviceSpec = {};
    std::vector<SDL_AudioStream*> streams;
};

#endif // GAME_AUDIO_H
