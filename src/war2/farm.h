#ifndef WAR2_FARM_H
#define WAR2_FARM_H

#include "game/sprite.h"
#include "game/unit.h"
#include "game/minimap.h"
#include "game/state.h"

class Farm : public Unit {
    public:
        Farm(Sprite* sprite);
        virtual void render(State* state) override;
    private:
        Sprite* body;
};

#endif
