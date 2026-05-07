#include "game/audio.h"

#include "game/sound.h"

#include <iostream>

Audio::Audio() {
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (device == 0) {
        std::cout << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return;
    }

    if (!SDL_GetAudioDeviceFormat(device, &deviceSpec, nullptr)) {
        std::cout << "Failed to get audio device format: " << SDL_GetError() << std::endl;
        SDL_CloseAudioDevice(device);
        device = 0;
        return;
    }

    if (!SDL_ResumeAudioDevice(device)) {
        std::cout << "Failed to resume audio device: " << SDL_GetError() << std::endl;
        SDL_CloseAudioDevice(device);
        device = 0;
    }
}

void Audio::play(const Sound* sound) {
    if (device == 0 || sound == nullptr || sound->buffer == nullptr || sound->length == 0) {
        return;
    }

    SDL_AudioStream* stream = SDL_CreateAudioStream(&sound->spec, &deviceSpec);
    if (stream == nullptr) {
        std::cout << "Failed to create audio stream: " << SDL_GetError() << std::endl;
        return;
    }

    if (!SDL_BindAudioStream(device, stream)) {
        std::cout << "Failed to bind audio stream: " << SDL_GetError() << std::endl;
        SDL_DestroyAudioStream(stream);
        return;
    }

    if (!SDL_PutAudioStreamData(stream, sound->buffer, static_cast<int>(sound->length))) {
        std::cout << "Failed to queue audio data: " << SDL_GetError() << std::endl;
        SDL_DestroyAudioStream(stream);
        return;
    }

    if (!SDL_FlushAudioStream(stream)) {
        std::cout << "Failed to flush audio stream: " << SDL_GetError() << std::endl;
        SDL_DestroyAudioStream(stream);
        return;
    }

    streams.push_back(stream);
}

void Audio::update() {
    for (auto it = streams.begin(); it != streams.end();) {
        SDL_AudioStream* stream = *it;
        if (stream == nullptr || SDL_GetAudioStreamQueued(stream) > 0) {
            ++it;
            continue;
        }

        SDL_DestroyAudioStream(stream);
        it = streams.erase(it);
    }
}

bool Audio::isReady() const {
    return device != 0;
}

Audio::~Audio() {
    for (SDL_AudioStream* stream : streams) {
        SDL_DestroyAudioStream(stream);
    }

    if (device != 0) {
        SDL_CloseAudioDevice(device);
    }
}
