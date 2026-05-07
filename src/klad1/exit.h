#ifndef KLAD1_EXIT_H
#define KLAD1_EXIT_H

#include "game/object.h"

class Context;

class Exit : public Object {
public:
    Exit(float x, float y);

    void render(Context* context) override;
};

#endif // KLAD1_EXIT_H
