#ifndef GAME_UNIT
#define GAME_UNIT

#include <deque>
#include <initializer_list>

#include <SDL3_image/SDL_image.h>

#include "game/object.h"
#include "game/job.h"

// Forward declaration of classes to avoid circular dependencies.
// class Object;
class Map;
class Minimap;
class Scene;
class State;
class Position;
class Cell;
class Terrain;
class Minimap;

/* Class 'Unit' extends the 'Object' class and represents a unit in a game.
   It includes methods for rendering the unit, updating its map cells,
   determining its ability to move, and managing its selection state.
*/
class Unit: public Object {
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

    int layer = 0;    // Layer where the unit exists in map

    uint16_t allowedTerrains = 0;  // Default: No terrain allowed
    bool ignoresTerrain = false;   // If true, unit ignores terrain checks
    // std::vector<std::pair<int, int>> cells; // Cells where unit exist in map

    // SDL_Color minimapColor;
    // Uint32 minimapColorCache = 0;

    SDL_Color color;

    public:
    bool moved;
    Map* map;
    Minimap* minimap;
    Scene* scene;
    // Render position is relateive to position but can be different
    // Like can be x:-24, y:-24 which means it will be drown
    // Sligthly up and left from parent position
    // While main .position represents object in map
    // .renderPosition determines where on screen the object is shown
    Position* renderPosition;
    // Select position defines select area it is also relative to .position
    Position* selectPosition;

    // Setter methods
    virtual Unit* setMap(Map* map);
    //virtual Unit* setScene(Map* map);

    // Render methods
    virtual void render(State* state) override;
    virtual SDL_FRect* getRenderPosition();

    //virtual void renderHitbox(State* state);

    // Map cell methods
    void updateGrid();
    bool canOccupy(float newX, float newY, float newWidth, float newHeight);
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


    // Map related
    virtual int getLayer();
    virtual void setLayer(int layer);

    // Terrain Management
    void allowTerrain(int terrainId);                 // Allow a single terrain
    void allowTerrains(std::initializer_list<int> terrains); // Allow multiple terrains
    void removeTerrain(int terrainId);               // Remove a terrain
    void ignoreTerrain();                                // Mark unit as flying
    bool isTerrainAllowed(int terrainId) const;      // Check if terrain is allowed

    // Minimap related
    virtual bool hasMinimap();
    // virtual Uint32 getMinimapColor(const SDL_PixelFormatDetails* format);
    // virtual void setMinimapColor(SDL_Color color);
    virtual void setColor(SDL_Color color);
    virtual SDL_Color* getColor();

    // Job related
    void updateJobs(State* state);
    void addJob(Job* job);
    void addJobs(std::initializer_list<Job*> jobs);
    void removeJob(int type);                     // <-- NEW
    void removeJobs(std::initializer_list<int>);  // <-- NEW
    void pauseJobs();   // call when unit is paused
    void resumeJobs();  // call when unit is resumed
    void renderJobs(State* state);
    virtual void onJobFinished(Job* job);

    // Game pause related
    virtual void pause();
    virtual void resume();

    // Debug related
    virtual void drawPosition(State* state);

    // Destructor
    ~Unit();
};

#endif // GAME_UNIT
