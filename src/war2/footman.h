#ifndef WAR2_FOOTMAN_H
#define WAR2_FOOTMAN_H

#include "game/unit.h"
#include "game/animation.h"
#include "game/state.h"
#include "game/camera.h"
#include "game/text.h"

#include "war2/enum.h"

class Footman : public Unit {
    public:
        Animation* body;
        // int maxSpeed = 100;
        int action = IDLE;
        int faceX = RIGHT;
        int faceY = DOWN;
        int cameraEdge = 100;
        int cameraStep = 1;
        bool cameraScroll = true;
        Text* text;
        
        // Constructor
        Footman(Sprite* sprite, TTF_Font* font);
        
        // Movement functions
        void rotate(float axisX, float axisY);
        // virtual bool move(int deltaTime, float moveX, float moveY);
        virtual void sync(uint32_t inAction, uint32_t inFaceX, uint32_t inFaceY);
        
        // Update and rendering functions
        virtual void update(State* state) override;
        virtual void cameraFollow(Camera* camera);
        virtual void render(State* state) override;
        void onJobFinished(Job* job) override;
        // virtual Uint32 getMinimapColor(SDL_PixelFormat* format);
        
        // Destructor
        ~Footman();
        
    private:
            bool syncing = false;
};

#endif // WAR2_FOOTMAN_H
