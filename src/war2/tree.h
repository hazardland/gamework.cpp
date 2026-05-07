#ifndef WAR2_TREE_H
#define WAR2_TREE_H

#include "game/sprite.h"
#include "game/unit.h"
#include "game/minimap.h"
#include "game/state.h"

class Tree : public Unit {
    public:
        Tree(Sprite* sprite);
        virtual void render(State* state) override;
    private:
        Sprite* body;
};

#endif
