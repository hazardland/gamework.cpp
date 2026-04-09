#include "game/sound.h"

#include "game/audio.h"
#include "game/context.h"

#include <iostream>

Sound::Sound(const char* path) {
    if (!SDL_LoadWAV(path, &spec, &buffer, &length)) {
        std::cout << "Failed to load sound " << path << ": " << SDL_GetError() << std::endl;
    }
}

void Sound::play(Context* context) const {
    if (context == nullptr || context->audio == nullptr) {
        return;
    }

    context->audio->play(this);
}

bool Sound::isReady() const {
    return buffer != nullptr && length > 0;
}

Sound::~Sound() {
    if (buffer != nullptr) {
        SDL_free(buffer);
    }
}
