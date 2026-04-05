#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include <SDL3_image/SDL_image.h>

class State;
class Position;
class Scene;

/*
    Methods in this class work with the actual game world position and size.
    Collision is calculated by the object's x, y, width, and height dimensions.
    The object's viewRect is equal to the object's position.
    If you want the game world position and size to be different than on screen
    (for example, a different hitbox than frame size) then use the Unit class
    which has a separate hitbox and viewbox.

    The id is allocated only when the id is requested.
*/

class Object {
    private:
        uint32_t id = 0;   // Object ID
        std::vector<Position*> childPositions; // Track child positions

    protected:
        Position* position;   // Object's position and size

    public:
        static uint32_t count;    // Keep count of total objects
        Object();  // Declare constructor
        // Object(Position* position);  // Declare constructor
        // Setter and getter methods
        void setId();
        uint32_t getId();
        bool hasId();
        virtual void addPosition(float x, float y);
        virtual void setPosition(float x, float y);
        virtual void setSize(float width, float height);
        virtual void setX(float x);
        virtual void setY(float y);
        virtual void setHeight(float height);
        virtual void setWidth(float width);
        virtual float getX();
        virtual float getY();
        virtual float getHeight();
        virtual float getWidth();
        virtual SDL_FRect* getPosition();
        virtual Position* createChildPosition(float x, float y, float width, float height);
        virtual Position* createChildPosition(float x, float y);
        void updateChildPositions();
        virtual bool isVisible(State* state);
        virtual void update(State* state);
        virtual void render(State* state);
        virtual ~Object();
};

#endif // GAME_OBJECT
