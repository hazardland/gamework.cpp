#include <random>
#include <vector>

#include "game/context.h"
#include "game/clock.h"
#include "game/camera.h"
#include "game/image.h"
#include "game/scene.h"
#include "game/unit.h"
#include "game/window.h"
#include "game/world.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class QueryBox : public Unit {
public:
    QueryBox(float vx, float vy, float worldWidth, float worldHeight)
        : vx(vx), vy(vy), worldWidth(worldWidth), worldHeight(worldHeight) {
        setLayer(0);
        ignoreTiles();
        setSize(16, 16);
        color = {40, 40, 40, 255};
    }

    void update(Context* context) override {
        color = {40, 40, 40, 255};

        float dx = vx * static_cast<float>(context->clock->delta) * 0.05f;
        float dy = vy * static_cast<float>(context->clock->delta) * 0.05f;

        float nextX = getX() + dx;
        float nextY = getY() + dy;

        if (nextX < 0 || nextX + getWidth() > worldWidth) {
            vx = -vx;
            dx = -dx;
        }

        if (nextY < 0 || nextY + getHeight() > worldHeight) {
            vy = -vy;
            dy = -dy;
        }

        addPosition(dx, dy);
    }

    void render(Context* context) override {
        SDL_FRect* rect = context->camera->translate(getPosition());
        SDL_SetRenderDrawColor(context->renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(context->renderer, rect);
    }

private:
    static SDL_Color randomColor() {
        static std::mt19937 rng(424242);
        static std::uniform_int_distribution<int> dist(80, 255);
        return {
            static_cast<Uint8>(dist(rng)),
            static_cast<Uint8>(dist(rng)),
            static_cast<Uint8>(dist(rng)),
            255
        };
    }

    float vx = 0;
    float vy = 0;
    float worldWidth = 0;
    float worldHeight = 0;
    SDL_Color color = {255, 255, 255, 255};
};

class QueryScene : public Scene {
public:
    using Scene::Scene;

    void prepare(Context* context) override {
        Scene::prepare(context);

        font = TTF_OpenFont("assets/war2/fonts/titillium.ttf", 18);
        TTF_SetFontOutline(font, 1);

        world = new World(
            new Image(renderer, "assets/klad1/sprites.png"),
            32,
            22,
            800,
            600,
            1
        );

        for (int y = 0; y < world->gridHeight; y++) {
            for (int x = 0; x < world->gridWidth; x++) {
                world->setCell(x, y, 0, 0);
            }
        }

        float width = world->getWidth();
        float height = world->getHeight();

        std::mt19937 rng(1337);
        std::uniform_real_distribution<float> posX(0.0f, width - 16.0f);
        std::uniform_real_distribution<float> posY(0.0f, height - 16.0f);
        std::uniform_real_distribution<float> speed(-2.5f, 2.5f);

        constexpr int BOX_COUNT = 100000;
        boxes.reserve(BOX_COUNT);

        for (int i = 0; i < BOX_COUNT; i++) {
            float vx = speed(rng);
            float vy = speed(rng);

            if (vx == 0) vx = 1;
            if (vy == 0) vy = -1;

            QueryBox* box = new QueryBox(vx, vy, width, height);
            box->setWorld(world);
            box->setPosition(posX(rng), posY(rng));
            addObject(box);
            boxes.push_back(box);
        }

        addFps(font);
        context->camera->setZoom(1.0f);
    }

    void clear() override {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        Scene::clear();
    }

private:
    TTF_Font* font = nullptr;
    std::vector<QueryBox*> boxes;
};

int main(int argc, char** argv) {
    Window* window = new Window("eachUnit test", 1280, 720);
    window->setScene(new QueryScene(window));
    return window->run();
}
