#ifndef GAME_UNIT
#define GAME_UNIT

#include <deque>
#include <functional>
#include <initializer_list>

#include <SDL3_image/SDL_image.h>

#include "game/job.h"
#include "game/object.h"
#include "game/world.h"

class Scene;
class Context;
class Position;

/* Class 'Unit' extends the 'Object' class and represents a unit in a game.
   It includes methods for rendering the unit, updating its map cells,
   determining its ability to move, and managing its selection state.
*/
class Unit: public Object {
    friend class World;
protected:
    bool paused = false;
    float speed = 15;
private:

    int gridFromX;
    int gridFromY;
    int gridToX;
    int gridToY;
    bool gridSet = false;
    std::deque<Job*> jobs;

    // int lastCellLeft, lastCellTop, lastCellRight, lastCellBottom;
    bool selected = false;

    int layer = 0;    // Layer where the unit exists in world

    uint16_t allowedTiles = 0;
    bool ignoresTiles = true;
    uint64_t lastScanId = 0;

    SDL_Color color;

    public:
    bool moved = false;
    World* world = nullptr;
    Scene* scene = nullptr;
    // Render position is relateive to position but can be different
    // Like can be x:-24, y:-24 which means it will be drown
    // Sligthly up and left from parent position
    // While main .position represents object in world
    // .renderPosition determines where on screen the object is shown
    Position* renderPosition = nullptr;
    // Select position defines select area it is also relative to .position
    Position* selectPosition = nullptr;

    // Setter methods
    virtual Unit* setWorld(World* world);

    // Render methods
    bool isVisible(Context* context) override;
    virtual void render(Context* context) override;
    virtual SDL_FRect* getRenderPosition();

    //virtual void renderHitbox(Context* context);

    // World cell methods
    void updateGrid();
    bool canPlace(float newX, float newY, float newWidth, float newHeight);
    bool canMove(float dx, float dy);

    // Position methods
    void setPosition(float x, float y) override;
    void addPosition(float x, float y) override;

    // Selection methods
    bool isSelected();
    void select();

    // Move related
    virtual void rotate(float deltaX, float deltaY); // optional, override per unit
    bool move(Uint64 deltaTime, float dirX, float dirY, float& deltaX, float& deltaY);
    bool move(Uint64 deltaTime, float dirX, float dirY); // <- new
    virtual bool canCrossUnit(Unit* target) const;
    virtual bool canCrossTile(int type) const;
    virtual int getType() const;

    bool scanUnits(float right, float top, float left, float bottom, const std::function<bool(Unit*)>& fn, int layer = -1) {
        if (world == nullptr || !isReady()) {
            return true;
        }

        float x = getX() - left;
        float y = getY() - top;
        float width = getWidth() + left + right;
        float height = getHeight() + top + bottom;
        int useLayer = (layer == -1) ? getLayer() : layer;
        return world->scanUnits(x, y, width, height, [&](Unit* unit) {
            if (unit == this) {
                return true;
            }
            return fn(unit);
        }, useLayer);
    }


    // World related
    virtual int getLayer();
    virtual void setLayer(int layer);

    void allowTile(int tileId);
    void allowTiles(std::initializer_list<int> tiles);
    void removeTile(int tileId);
    void ignoreTiles();
    bool isTileAllowed(int tileId) const;
    bool touchesTile(int type, float offsetX = 0, float offsetY = 0, float offsetWidth = 0, float offsetHeight = 0);
    bool touchesUnit(int kind, float right = 0, float top = 0, float left = 0, float bottom = 0);

    virtual void setColor(SDL_Color color);
    virtual SDL_Color* getColor();

    // Job related
    void updateJobs(Context* context);
    void addJob(Job* job);
    void addJobs(std::initializer_list<Job*> jobs);
    void removeJob(int type);                     // <-- NEW
    void removeJobs(std::initializer_list<int>);  // <-- NEW
    void pauseJobs();   // call when unit is paused
    void resumeJobs();  // call when unit is resumed
    void renderJobs(Context* context);
    virtual void onJobFinished(Job* job);

    // Game pause related
    virtual void pause();
    virtual void resume();

    // Destructor
    ~Unit();
};

#endif // GAME_UNIT


