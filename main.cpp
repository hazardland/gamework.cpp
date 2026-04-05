#include<stdio.h>

#include <game/window.h>

#include <war2/war2.h>


int main(int argc, char** argv){
    printf("Starting...\n");
    Window* window = new Window("", 1600, 900);
    window->setScene(new Warcraft(window));
    return window->run();
}
 
