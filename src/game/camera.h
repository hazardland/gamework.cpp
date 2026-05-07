/**
 * @file
 * @brief Contains the definition of the Camera class
 */

#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include <SDL3/SDL.h>
#include "game/cooldown.h"

/**
 * @class Camera
 * @brief A class that handles camera tracking in the game.
 *
 * Camera holds position and size of curreent visible rectangle inside game world
 * It can tell if the object's position is visible inside camera region
 * It can also translate absolute position from game world into screen position
 */
class Camera {
    private:

    int x=0;           ///< The x-coordinate of the camera
    int y=0;           ///< The y-coordinate of the camera
    int width;         ///< The width of the camera view
    int height;        ///< The height of the camera view

    SDL_FRect result;  ///< The rectangle representing the resulting camera view after translation

    float zoom = 1;
    const float zoomStep = 0.2f;  // Increment for zooming
    const float minZoom = 0.2f;
    const float maxZoom = 10.0f;
    Cooldown* zoomCooldown;

    public:
    Camera();
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    float getZoom();
    void setX(int newX);
    void setY(int newY);
    void addX(int value);
    void addY(int value);
    void setWidth(int newWidth);
    void setHeight(int newHeight);
    void setZoom(float newZoom);
    void zoomIn();
    void zoomOut();

    void setPosition(int x, int y);

    /**
     * @brief Sets the size of the camera view.
     * @param width The width of the camera view.
     * @param height The height of the camera view.
     */
    void setSize(int width, int height);

    /**
     * @brief Checks whether a position is visible inside the camera view.
     * @param position The position of the game object.
     * @return true if the game object is visible, false otherwise.
     */
    bool isVisible(SDL_FRect* position);

    /**
     * @brief Translates the position of a game object based on the camera view.
     * @param position The position of the game object.
     * @return A pointer to the resulting rectangle after translation.
     */
    SDL_FRect* translate(SDL_FRect* position);
};

#endif


