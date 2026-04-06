#include <stdio.h>

#include <game/window.h>

#include <klad1/klad1.h>

int main(int argc, char** argv) {
    printf("Starting Klad1...\n");
    Window* window = new Window("", 1536, 726);
    window->setScene(new Klad1(window));
    return window->run();
}
