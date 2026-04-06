#ifndef KLAD1_PLAYER_H
#define KLAD1_PLAYER_H

#include "game/unit.h"

class Animation;
class Sprite;
class State;
class Text;
typedef struct TTF_Font TTF_Font;

class Player : public Unit {
public:
    Player(Sprite* sprite, TTF_Font* font);

    void update(State* state) override;
    void render(State* state) override;

    ~Player();

private:
    static constexpr int IDLE = 1;
    static constexpr int CLIMB = 2;
    static constexpr int RUN_LEFT = 3;
    static constexpr int RUN_RIGHT = 4;

    Animation* body = nullptr;
    Text* debug = nullptr;

    bool insideLadder();
};

#endif // KLAD1_PLAYER_H
