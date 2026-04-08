// File: state.h

#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

/**
 * @class State
 * @brief Represents the state of the game and manages it in a single object.
 *
 * State class contains instances of Clock, Event, and Camera classes and
 * the SDL_Renderer object, which are all important components of the game state.
 */
class Screen {
public:
    int width;
    int height;
    Screen();
    int getWidth();
    int getHeight();
    void setSize(int width, int height);

};

#endif // GAME_SCREEN_H


