#ifndef WAR2_CRITTER_H
#define WAR2_CRITTER_H

#include "game/animation.h"
#include "game/unit.h"
#include "game/minimap.h"
#include "game/state.h"
#include "game/text.h"

#include "war2/enum.h"


class Critter : public Unit {
    public:

        Critter(Sprite* sprite, TTF_Font* font);
        virtual void update(State* state) override;
        virtual void render(State* state) override;
        void rotate(float axisX, float axisY) override;
    
    private:
        Animation* body;
        Text* debug;
        int faceX = 0;
        int faceY = DOWN;
        // int travelX;
        // int travelY;
        int travelsFinished = 0;
        void onJobFinished(Job* job) override;
};

#endif // CRITTER_HPP
