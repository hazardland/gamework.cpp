#include "game/camera.h"
#include <algorithm> // For std::min, std::max

Camera::Camera() {
    zoomCooldown = new Cooldown(150);
}

void Camera::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Camera::setSize(int width, int height) {
    this->width = width;
    this->height = height;
}

// Check if the given object is visible within the camera viewport
bool Camera::isVisible(SDL_FRect* position) {
    if (position->x + position->w > x  &&
        position->y + position->h > y  &&
        position->x < x + width / zoom &&
        position->y < y + height / zoom) {
        return true;
    }
    return false;
}

// Translate world coordinates to camera coordinates, applying zoom
SDL_FRect* Camera::translate(SDL_FRect* position) {
    result.x = (position->x - x) * zoom;
    result.y = (position->y - y) * zoom;
    result.w = position->w * zoom;
    result.h = position->h * zoom;
    return &result;
}

int Camera::getX() {

    return x; // Camera's top-left X position remains the same
}

int Camera::getY() {
    return y; // Camera's top-left Y position remains the same
}

int Camera::getWidth() {
    return static_cast<int>(width / zoom); // Adjust width based on zoom
}

int Camera::getHeight() {
    return static_cast<int>(height / zoom); // Adjust height based on zoom
}

void Camera::setX(int newX) {
    x = newX;
}

void Camera::addX(int value) {
    x += value;
}

void Camera::setY(int newY) {
    y = newY;
}

void Camera::addY(int value) {
    y += value;
}


void Camera::setWidth(int newWidth) {
    width = newWidth; // Scale width based on zoom
}

void Camera::setHeight(int newHeight) {
    height = newHeight; // Scale height based on zoom
}

void Camera::setZoom(float newZoom) {
    zoom = std::max(minZoom, std::min(newZoom, maxZoom)); // Clamp zoom between 0.1x and 10x
}

void Camera::zoomIn() {
    auto now = std::chrono::steady_clock::now();
    if (zoomCooldown->isReady()) {

        int lastWidth = getWidth();
        int lastHeight = getHeight();
        zoom = std::min(zoom + zoomStep, maxZoom);

        x += (lastWidth-getWidth())/2;
        y += (lastHeight-getHeight())/2;

        zoomCooldown->reset();
    }
}

void Camera::zoomOut() {
    auto now = std::chrono::steady_clock::now();
    if (zoomCooldown->isReady()) {

        int lastWidth = getWidth();
        int lastHeight = getHeight();

        zoom = std::max(zoom - zoomStep, minZoom);

        x += (lastWidth-getWidth())/2;
        y += (lastHeight-getHeight())/2;

        zoomCooldown->reset();

    }
}

float Camera::getZoom() {
    return zoom;
}


