#include <game/window.h>
#include <game/scene.h>
#include <game/cooldown.h>
#include <game/text.h>
#include <game/state.h>
#include <game/input.h>

#include <SDL3_ttf/SDL_ttf.h>

#include <deque>
#include <utility>
#include <cstdlib>
#include <string>

static const int   COLS = 20;
static const int   ROWS = 20;
static const float CELL = 28.0f;
static const float PAD  = 2.0f;

class SnakeScene : public Scene {
    using Scene::Scene;

public:
    std::deque<std::pair<int,int>> snake;
    std::pair<int,int> food = {0, 0};

    int dx = 1,  dy = 0;   // active direction
    int ndx = 1, ndy = 0;  // queued direction

    bool gameOver = false;
    int  score    = 0;

    Cooldown* stepTimer  = nullptr;
    Text*     scoreText  = nullptr;
    Text*     overlayText = nullptr;
    TTF_Font* font       = nullptr;

    // ---------------------------------------------------------------

    void spawnFood() {
        while (true) {
            int fx = rand() % COLS;
            int fy = rand() % ROWS;
            bool occupied = false;
            for (auto& seg : snake) {
                if (seg.first == fx && seg.second == fy) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                food = {fx, fy};
                return;
            }
        }
    }

    void initGame() {
        snake.clear();
        dx = 1;  dy = 0;
        ndx = 1; ndy = 0;
        gameOver = false;
        score    = 0;

        int cx = COLS / 2;
        int cy = ROWS / 2;
        snake.push_back({cx,     cy});
        snake.push_back({cx - 1, cy});
        snake.push_back({cx - 2, cy});

        spawnFood();

        if (stepTimer) stepTimer->reset();
    }

    void step() {
        dx = ndx;
        dy = ndy;

        int nx = snake.front().first  + dx;
        int ny = snake.front().second + dy;

        // Wall collision
        if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) {
            gameOver = true;
            return;
        }

        // Self collision (skip last segment — it vacates this tick)
        for (int i = 0; i < (int)snake.size() - 1; i++) {
            if (snake[i].first == nx && snake[i].second == ny) {
                gameOver = true;
                return;
            }
        }

        snake.push_front({nx, ny});

        if (nx == food.first && ny == food.second) {
            score++;
            spawnFood();
            // tail stays → snake grows
        } else {
            snake.pop_back();
        }
    }

    // ---------------------------------------------------------------

    virtual void prepare(State* state) override {
        font = TTF_OpenFont("assets/fonts/titillium.ttf", 18);

        scoreText = new Text(font, "Score: 0", 8, 8, {255, 255, 255, 255});
        scoreText->setPositionFixed(true);

        int ox = (int)(COLS * CELL / 2) - 155;
        int oy = (int)(ROWS * CELL / 2) - 10;
        overlayText = new Text(font, "GAME OVER  |  Space to restart", ox, oy, {255, 80, 80, 255});
        overlayText->setPositionFixed(true);

        stepTimer = new Cooldown(150);
        initGame();
    }

    virtual void update(State* state) override {
        auto* kb = state->input->keyboard;

        if (gameOver) {
            if (kb->space) initGame();
            return;
        }

        // Direction — 180° reversal blocked
        if ((kb->up    || kb->w) && dy == 0) { ndx =  0; ndy = -1; }
        if ((kb->down  || kb->s) && dy == 0) { ndx =  0; ndy =  1; }
        if ((kb->left  || kb->a) && dx == 0) { ndx = -1; ndy =  0; }
        if ((kb->right || kb->d) && dx == 0) { ndx =  1; ndy =  0; }

        if (stepTimer->isReady()) {
            step();
            stepTimer->reset();
        }
    }

    virtual void render(State* state) override {
        // Background
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        clear();

        // Grid cells
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        for (int col = 0; col < COLS; col++) {
            for (int row = 0; row < ROWS; row++) {
                SDL_FRect r = {col * CELL + PAD, row * CELL + PAD,
                               CELL - PAD * 2,   CELL - PAD * 2};
                SDL_RenderFillRect(renderer, &r);
            }
        }

        // Food
        SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);
        SDL_FRect fr = {food.first  * CELL + PAD, food.second * CELL + PAD,
                        CELL - PAD * 2,            CELL - PAD * 2};
        SDL_RenderFillRect(renderer, &fr);

        // Snake body (indices 1..N)
        SDL_SetRenderDrawColor(renderer, 60, 160, 60, 255);
        for (int i = 1; i < (int)snake.size(); i++) {
            SDL_FRect s = {snake[i].first  * CELL + PAD, snake[i].second * CELL + PAD,
                           CELL - PAD * 2,                CELL - PAD * 2};
            SDL_RenderFillRect(renderer, &s);
        }

        // Snake head (index 0)
        if (!snake.empty()) {
            SDL_SetRenderDrawColor(renderer, 100, 220, 100, 255);
            SDL_FRect h = {snake.front().first  * CELL + PAD, snake.front().second * CELL + PAD,
                           CELL - PAD * 2,                     CELL - PAD * 2};
            SDL_RenderFillRect(renderer, &h);
        }

        // Score
        scoreText->setText("Score: " + std::to_string(score));
        scoreText->render(state);

        // Game-over overlay
        if (gameOver) {
            overlayText->render(state);
        }

        present();
    }
};

// ---------------------------------------------------------------

int main(int argc, char** argv) {
    int winW = (int)(COLS * CELL);
    int winH = (int)(ROWS * CELL);
    Window* window = new Window("Snake", winW, winH);
    window->setScene(new SnakeScene(window));
    return window->run();
}
