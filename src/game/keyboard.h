#ifndef GAME_KEYBOARD_H
#define GAME_KEYBOARD_H

#include <SDL3/SDL.h>

/**
 * @class Keyboard
 * Represents a keyboard state at a certain moment.
 */
class Keyboard {
    public:
        bool up = false;  ///< Boolean representing if 'up' key is pressed.
        bool down = false;  ///< Boolean representing if 'down' key is pressed.
        bool left = false;  ///< Boolean representing if 'left' key is pressed.
        bool right = false;  ///< Boolean representing if 'right' key is pressed.
        bool space = false;  ///< Boolean representing if 'space' key is pressed.
        bool plus = false;  ///< Boolean representing if 'space' key is pressed.
        bool minus = false;  ///< Boolean representing if 'space' key is pressed.
        bool alt = false;
        bool enter = false;
        bool w = false;
        bool s = false;
        bool a  = false;
        bool d  = false;
        bool f5  = false;
        bool f11  = false;
        bool f12  = false;
        bool tab  = false;
        const bool* keys = nullptr;
        void read(const bool* keys);
    };

#endif


