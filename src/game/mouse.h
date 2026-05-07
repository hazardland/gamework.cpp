#ifndef GAME_MOUSE_H
#define GAME_MOUSE_H

#include <SDL3/SDL.h>

/**
 * @class Mouse
 * Represents a mouse state at a certain moment.
 */
class Mouse {
    public:
        float x = 0;
        float y = 0;
        bool rightDragActive = false;  ///< Boolean representing if right drag is active.
        bool rightDragEnded = false;  ///< Boolean representing if right drag has ended.
        int rightDragStartX = -1;  ///< X position where right drag started.
        int rightDragStartY = -1;  ///< Y position where right drag started.
        bool rightClick = false;  ///< Boolean representing if right click is active.
        bool leftDragActive = false;  ///< Boolean representing if left drag is active.
        bool leftDragEnded = false;  ///< Boolean representing if left drag has ended.
        int leftDragStartX = -1;  ///< X position where left drag started.
        int leftDragStartY = -1;  ///< Y position where left drag started.
        bool leftClick = false;  ///< Boolean representing if left click is active.
        void reset();
        void move(float x, float y);
        void down(int button);
        void up(int button);
        bool inside(SDL_FRect* position);  ///< Checks if the mouse is inside a certain area defined by a SDL_FRect.
    };

#endif


