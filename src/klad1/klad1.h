#ifndef KLAD1_SCENE_H
#define KLAD1_SCENE_H

#include "game/scene.h"

class Klad1 : public Scene {
    using Scene::Scene;

public:
    void prepare(State* state) override;
    void update(State* state) override;
};

#endif // KLAD1_SCENE_H
