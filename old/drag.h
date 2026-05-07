#ifndef GAME_DRAG_H
#define GAME_DRAG_H

#include <SDL3_image/SDL_image.h>

class Context;

/**
 * @brief Provides drag functionality for an SDL_FRect object.
 *
 * It checks mouse state for drag activity and updates the position of the SDL_FRect object accordingly.
 */
class Drag {
    private:
        int dragStartX;
        int dragStartY;
        SDL_FRect* position;
        bool enabled = true;

    public:
        bool dragging = false; ///< Indicates if the drag is currently in process.
        bool dragLeft = true;  ///< Indicates if the object can be dragged with the left mouse button.
        bool dragRight = true; ///< Indicates if the object can be dragged with the right mouse button.

        /**
         * @brief Constructs a Drag object.
         *
         * @param position The position of the object that can be dragged.
         * @param dragLeft Specifies if the object can be dragged with the left mouse button.
         * @param dragRight Specifies if the object can be dragged with the right mouse button.
         */
        Drag(SDL_FRect* position, bool dragLeft, bool dragRight);

        /**
         * @brief Updates the position of the object based on mouse activity.
         *
         * @param state The game state, used to get the current mouse state.
         */
        void update(Context* context);

        /**
         * @brief Disables the drag functionality for the object.
         */
        void disable();

        /**
         * @brief Enables the drag functionality for the object.
         */
        void enable();
};

#endif // GAME_DRAG_H


