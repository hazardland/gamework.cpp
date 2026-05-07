// File: state.cpp

#include "game/screen.h"

Screen::Screen() {

}

int Screen::getWidth() {
    return width;
}

int Screen::getHeight() {
    return height;
}

void Screen::setSize(int width, int height) {
    this->width = width;
    this->height = height;
}


