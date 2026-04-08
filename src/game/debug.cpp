#include "game/debug.h"

#include "game/camera.h"
#include "game/context.h"
#include "game/text.h"

namespace {
Context* debugState = nullptr;
TTF_Font* debugFont = nullptr;
Text* debugText = nullptr;
}

void debug(Context* context, TTF_Font* font) {
    debugState = context;

    if (font == nullptr || font == debugFont) {
        return;
    }

    debugFont = font;

    if (debugText != nullptr) {
        delete debugText;
    }

    debugText = new Text(debugFont, "", 0, 0);
}

void draw(SDL_FRect* rect, SDL_Color color) {
    if (debugState == nullptr || rect == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(
        debugState->renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );
    SDL_RenderRect(debugState->renderer, debugState->camera->translate(rect));
    SDL_SetRenderDrawColor(debugState->renderer, 0, 0, 0, 0);
}

void printLines(SDL_FRect* rect, const std::vector<std::string>& lines) {
    if (debugState == nullptr || debugFont == nullptr || debugText == nullptr || rect == nullptr) {
        return;
    }

    SDL_FRect screenRect = *debugState->camera->translate(rect);
    float lineHeight = static_cast<float>(TTF_GetFontHeight(debugFont) + 2);
    float textY = screenRect.y - lineHeight * static_cast<float>(lines.size());

    for (const std::string& line : lines) {
        debugText->setText(line);
        debugText->setPosition(screenRect.x, textY);
        debugText->render(debugState);
        textY += lineHeight;
    }
}



