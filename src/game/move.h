#ifndef GAME_MOVE_H
#define GAME_MOVE_H

#include "game/job.h"
#include "game/unit.h"

class Move : public Job {
public:
    static constexpr int TYPE = 1;

    Move(Unit* unit, float travelX, float travelY);
    ~Move();

    bool update(Context* context) override;
    void render(Context* context) override;
    std::vector<Job*> finish() override;

    bool isUnique() const override { return true; }
    int getType() const override { return TYPE; } // developer-defined consistent ID
private:
    Unit* unit;
    float travelX = 0;
    float travelY = 0;
};

#endif // GAME_MOVE_H

