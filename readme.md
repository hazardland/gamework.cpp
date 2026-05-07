# GameWork — C++ 2D Game Framework Manual

GameWork is a 2D game framework built on top of **SDL3**. It handles the boilerplate of windowing, rendering, sprite animation, tile maps, input, a job-based task system, and WebSocket networking so you can focus on writing game logic.

> Built and tested with MinGW64 on Windows and GCC on Linux.

---

> **Note on `src/examples/`:** The repository contains a demo game in `src/examples/` that was built to test and showcase the framework. It is **not part of the framework** — all framework code lives exclusively under `src/game/`. The examples folder may be removed at any time. This manual documents only the framework API.

---

## Table of Contents

1. [Installation](#1-installation)
2. [Core Concepts — Window, Scene, State](#2-core-concepts--window-scene-state)
3. [Images — Loading and Rendering Textures](#3-images--loading-and-rendering-textures)
4. [Sprites — Texture Atlases and Clips](#4-sprites--texture-atlases-and-clips)
5. [Animation — Delta-Time Frame Playback](#5-animation--delta-time-frame-playback)
6. [Objects — The Base Game Entity](#6-objects--the-base-game-entity)
7. [Units — Interactive Game Entities](#7-units--interactive-game-entities)
8. [Map and Terrain — Tile-Based World](#8-map-and-terrain--tile-based-world)
9. [Camera — Viewport and Zoom](#9-camera--viewport-and-zoom)
10. [Input — Keyboard and Mouse](#10-input--keyboard-and-mouse)
11. [Jobs — Task Queue System](#11-jobs--task-queue-system)
12. [Minimap](#12-minimap)
13. [UI Primitives — Text, Rectangle, Select, FPS](#13-ui-primitives--text-rectangle-select-fps)
14. [Networking — WebSocket Client](#14-networking--websocket-client)
15. [Complete Example — Warcraft Scene](#15-complete-example--warcraft-scene)

---

## 1. Installation

### Linux (Ubuntu/Debian)

```bash
sudo apt install g++ make \
  libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev \
  libwebsockets-dev libssl-dev zlib1g-dev

make
./main
```

### Windows (MSYS2 / MinGW64)

Install MSYS2 from https://www.msys2.org/, then in the **MinGW64** shell:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-pkg-config \
          mingw-w64-x86_64-make \
          mingw-w64-x86_64-sdl3 \
          mingw-w64-x86_64-sdl3-image \
          mingw-w64-x86_64-sdl3-ttf \
          mingw-w64-x86_64-libwebsockets
```

`make` is shipped as `mingw32-make.exe`, rename it so the Makefile works:

```bash
copy D:\app\msys64\mingw64\bin\mingw32-make.exe D:\app\msys64\mingw64\bin\make.exe
```

Add these three directories to your Windows PATH:

```
D:\app\msys64\ucrt64\bin
D:\app\msys64\mingw64\bin
D:\app\msys64\usr\bin
```

Then build and run:

```bash
make
./main.exe
```

To check which DLLs the executable needs at runtime:

```bash
objdump -p main.exe | grep DLL
```

---

## 2. Core Concepts — Window, Scene, State

Every GameWork program follows the same skeleton: create a **Window**, attach a **Scene**, and call `run()`.

```cpp
#include <game/window.h>
#include <game/scene.h>

class MyScene : public Scene {
    using Scene::Scene;
public:
    virtual void prepare(State* state) {
        // Called once before the loop starts. Load assets here.
    }

    virtual void update(State* state) {
        // Called every frame. Move things, check input, run logic.
        Scene::update(state); // always call super — it updates all objects
    }

    virtual void render(State* state) {
        // Called every frame after update. Draw things.
        Scene::render(state); // always call super — it draws all objects
    }
};

int main(int argc, char** argv) {
    Window* window = new Window("My Game", 800, 600);
    window->setScene(new MyScene(window));
    return window->run();
}
```

### Window

`Window` owns the SDL window, manages fullscreen, and runs the main loop.

| Method | Description |
|--------|-------------|
| `Window(title, w, h)` | Create a window |
| `setScene(Scene*)` | Attach the active scene |
| `run()` | Start the game loop; returns 0 when the window is closed |
| `toggleFullscreen()` | Toggle fullscreen (also triggered by Alt+Enter in your scene) |

### Scene

`Scene` is the container for all your game objects, sprites, and the map. You subclass it and override three virtual methods.

| Method | Description |
|--------|-------------|
| `prepare(State*)` | One-time init (load images, create sprites, spawn objects) |
| `update(State*)` | Per-frame logic. Call `Scene::update(state)` to propagate to objects |
| `render(State*)` | Per-frame drawing. Call `Scene::render(state)` to draw all objects |
| `addObject(Object*)` | Register an object (rendered and updated automatically) |
| `addObject(Object*, id)` | Register with a specific integer ID |
| `getObject(id)` | Look up a registered object by ID |
| `removeObject(Object*)` | Unregister and stop updating/drawing |

`Scene` also exposes:
- `sprites` — `std::map<int, Sprite*>` for loading sprites once and sharing them
- `map` — pointer to the active `Map`
- `minimap` — pointer to the `Minimap`
- `renderer` — the `SDL_Renderer*` needed to construct `Image` and `Text`

### State

`State` is passed to every `update()` and `render()` call. It is a read-only snapshot of the current frame's context — never create or own one yourself, just use what is passed in.

```cpp
state->clock->delta   // milliseconds since last frame (float)
state->clock->fps     // current FPS
state->camera         // Camera* — viewport position and zoom
state->input->keyboard // Keyboard* — key states
state->input->mouse    // Mouse* — mouse position and clicks
state->client          // Client* — WebSocket client (may be nullptr)
```

---

## 3. Images — Loading and Rendering Textures

`Image` wraps an `SDL_Texture`. It loads a PNG from disk and uploads it to the GPU once.

```cpp
#include <game/image.h>

// In Scene::prepare:
Image* image = new Image(renderer, "assets/sprites/planet.png");
```

To render a region of the image (a "frame") at a position on screen:

```cpp
// A crop region inside the texture (in pixels)
SDL_Rect frame = { 0, 0, 100, 100 };

// Where to draw it on screen (x, y, width, height)
SDL_FRect position = { 300, 200, 200, 200 };

// In render():
image->render(&frame, &position);
```

`Image` also supports **color replacement** before the texture is uploaded, used internally by the map system to recolor tiles.

---

## 4. Sprites — Texture Atlases and Clips

A **sprite sheet** is one image that contains many animation frames. `Sprite` describes how to read it.

### Creating a Sprite

```cpp
#include <game/sprite.h>
#include <game/image.h>

// planet.png is a 6-column × 4-row sheet where each frame is 100×100 px
Sprite* sprite = new Sprite(
    new Image(renderer, "doc/images/planet.png"),
    100,   // frame width
    100,   // frame height
    60     // ms per frame (default pause)
);
```

The fourth argument (`readVertically`) can be set to `true` if the sheet is read column-first instead of row-first.

### Adding Clips

A **clip** is a named range of frames within the sprite sheet.

```cpp
sprite->addClip(
    1,   // clip ID (any integer you choose)
    1,   // start row  (1-based)
    1,   // start cell (1-based)
    24   // number of frames
);
```

`addClip` returns the `Sprite*` so you can chain:

```cpp
sprites[MY_SPRITE] = (new Sprite(new Image(renderer, "grunt.png"), 72, 72, 100, true))
    ->addClip(MOVE+UP,    1, 2, 4)
    ->addClip(MOVE+DOWN,  5, 2, 4)
    ->addClip(MOVE+LEFT,  3, 2, 4, true, false)  // flipX=true
    ->addClip(IDLE+DOWN,  5, 1, 1);
```

The last two parameters of `addClip` are `flipX` and `flipY` (both default `false`). This lets you share one directional animation strip and flip it to get the mirrored direction.

### Storing Sprites in the Scene

Sprites are expensive to load and are shared between many objects. Put them in `Scene::sprites` so they are loaded once:

```cpp
// In prepare():
sprites[SPRITE_FOOTMAN] = (new Sprite(...))->addClip(...);

// When creating a unit:
new Footman(sprites[SPRITE_FOOTMAN]);
```

---

## 5. Animation — Delta-Time Frame Playback

Raw frame-counting ties your animation speed to your FPS. `Animation` uses **delta time** (elapsed milliseconds) so animations always play at the same speed regardless of frame rate.

```cpp
#include <game/animation.h>

// Create an animation with a sprite and a default clip ID
Animation* animation = new Animation(sprite, IDLE_CLIP);
```

In your `update()`:

```cpp
animation->update(state->clock->delta); // advance the frame timer
```

In your `render()`:

```cpp
animation->render(&position); // draw the current frame
```

To change what clip is playing:

```cpp
animation->play(MOVE_CLIP);               // switch clip
animation->play(MOVE_CLIP, speedFactor);  // also set playback speed multiplier
```

---

## 6. Objects — The Base Game Entity

`Object` is the base class for everything that lives in the scene. It has a position, a size, and three virtual lifecycle methods.

```cpp
#include <game/object.h>

class Planet : public Object {
public:
    Animation* animation;

    Planet(Sprite* sprite) {
        animation = new Animation(sprite, SPIN_CLIP);
        setSize(200, 200);
    }

    virtual void update(State* state) override {
        // Center on screen
        setPosition(
            state->camera->getWidth()  / 2 - getWidth()  / 2,
            state->camera->getHeight() / 2 - getHeight() / 2
        );
        animation->update(state->clock->delta);
    }

    virtual void render(State* state) override {
        animation->render(getPosition());
    }
};
```

### Position API

| Method | Description |
|--------|-------------|
| `setPosition(x, y)` | Move to absolute position |
| `setSize(w, h)` | Set width and height |
| `addPosition(dx, dy)` | Relative move |
| `getX()`, `getY()` | Current coordinates |
| `getWidth()`, `getHeight()` | Dimensions |
| `getPosition()` | Returns `SDL_FRect*` |

### Visibility

```cpp
virtual bool isVisible(State* state);  // override to hide/show conditionally
```

### Child Positions

Objects can have child positions for rendering offsets or hit-boxes:

```cpp
// In a Unit constructor, create a visual offset (larger sprite, smaller logical size)
renderPosition = createChildPosition(-24, -24, 72, 72);

// Create a floating label above the unit
text = new Text(font, createChildPosition(-10, -40));
```

Child positions update automatically when the parent moves.

---

## 7. Units — Interactive Game Entities

`Unit` extends `Object` with everything needed for in-world entities: terrain awareness, layer system, job queue, minimap color, and selection.

```cpp
#include <game/unit.h>

class MyUnit : public Unit {
public:
    Animation* body;

    MyUnit(Sprite* sprite) {
        setLayer(1);               // 0=water, 1=ground, 2=air
        allowTerrains({1, 2});     // terrain IDs this unit can walk on
        setSize(24, 24);
        speed = 50;                // movement speed (pixels/100ms)
        body = new Animation(sprite, IDLE_DOWN);
        renderPosition = createChildPosition(-24, -24, 72, 72);
        setColor({255, 255, 0, 255}); // dot color on minimap
    }
    ...
};
```

### Attaching a Unit to the Map

```cpp
unit->setMap(map);
unit->setPosition(x, y);
addObject(unit, unit->getId());
```

### Terrain and Collision

```cpp
// Check if a region is free to occupy (respects terrain and other units)
if (unit->canOccupy(x, y, w, h)) {
    unit->setPosition(x, y);
}

// Move smoothly in a direction (returns true if the move succeeded)
unit->move(state->clock->delta, moveX, moveY);
```

### Layers

Three layers keep different unit types separate:

| Value | Constant | Typical use |
|-------|----------|-------------|
| 0 | `LAYER_WATER` | Boats, fish |
| 1 | `LAYER_GROUND` | Infantry, buildings |
| 2 | `LAYER_AIR` | Aircraft |

### Selection

```cpp
unit->select();          // mark as selected
unit->isSelected();      // returns bool
unit->drawPosition(state); // draw selection indicator (call in render)
```

### Camera Follow

```cpp
// From inside a Unit::update()
void cameraFollow(Camera* camera) {
    camera->setX(getX() + getWidth() / 2 - camera->getWidth() / 2);
    camera->setY(getY() + getHeight() / 2 - camera->getHeight() / 2);
}
```

---

## 8. Map and Terrain — Tile-Based World

`Map` is a tile-based world made of `Cell` objects arranged in a 2D grid.

### Creating a Map

```cpp
#include <game/map.h>
#include <game/terrain.h>

// Image is the tile sprite sheet, 32×32 pixels per cell, 500×500 grid, 3 layers
map = new Map(
    new Image(renderer, "assets/sprites/winter.png"),
    32, 32,     // cell width, cell height in pixels
    500, 500,   // grid width, grid height in cells
    3,          // number of layers
    font        // optional font for debug labels
);
```

### Defining Terrain Types

```cpp
map->terrains = {
    new Terrain(TERRAIN_WATER,  LAYER_WATER,  {51,  51,  255}), // blue on minimap
    new Terrain(TERRAIN_SHORE,  LAYER_GROUND, {102, 178, 215}),
    new Terrain(TERRAIN_GROUND, LAYER_GROUND, {255, 255, 255}),
    new Terrain(TERRAIN_FOREST, LAYER_GROUND, {34,  139, 34 }),
};
```

`Terrain` constructor: `Terrain(id, layer, SDL_Color minimap_color)`

### Defining Tile Variants

The tile map resolves which sprite frame to use given the terrain pattern of a cell's four corners. Keys are 4-digit codes where each digit is a terrain ID; values are lists of frame indices to pick from randomly.

```cpp
map->tiles = {
    {0,    {319, 320, 321}},   // pure water — pick any of these frames
    {2222, {349, 350, 351}},   // pure ground
    {1222, {259, 260}},        // ground/ice transition NW corner
    ...
};
```

### Procedural Generation

```cpp
// seed: random seed
// intensity: 0.01–0.1, lower = larger features
// ranges: noise thresholds per terrain (one float per terrain type, last must be 1.0)
map->generate(seed, 0.05, {0.2, 0.45, 0.8, 1.0});
```

Uses OpenSimplexNoise internally. Calling `generate()` again with a new seed replaces the terrain.

### Importing from a 2D Array

Useful for hand-crafted or Tiled-exported maps:

```cpp
map->import({
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 1, 1},
    {0, 0, 0, 0},
});
```

Each number is a terrain index into `map->terrains`.

### Debug Visualization

```cpp
map->setDebug(true);    // show grid lines and cell info
map->toggleDebug();     // toggle
```

### Smooth Tile Transitions

The map automatically selects edge/corner transition tiles based on which terrain neighbors each cell. This is what makes coast lines, forest edges, etc. look clean without hard pixel borders. The algorithm evaluates the four corners of each cell, builds a 4-digit pattern code, and looks it up in `map->tiles`.

---

## 9. Camera — Viewport and Zoom

The `Camera` defines what portion of the world is currently visible on screen.

```cpp
state->camera->setX(500);   // scroll to x=500 in world space
state->camera->setY(300);
state->camera->addX(5);     // scroll right by 5 pixels
state->camera->addY(-5);    // scroll up by 5 pixels
```

### Coordinate Translation

World positions need to be translated to screen positions before rendering:

```cpp
// In a Unit::render():
SDL_FRect* screenPos = state->camera->translate(getRenderPosition());
animation->render(screenPos);
```

### Visibility Culling

Skip rendering objects outside the viewport:

```cpp
if (state->camera->isVisible(getRenderPosition())) {
    animation->render(state->camera->translate(getRenderPosition()));
}
```

`Scene::render()` does this automatically for objects added with `addObject()`. You only need it if you render something manually.

### Zoom

```cpp
state->camera->zoomIn();         // step zoom in
state->camera->zoomOut();        // step zoom out
state->camera->setZoom(2.0f);    // set exact zoom level (0.2x – 10x)
```

Zoom changes have a built-in cooldown to prevent jumping on held keys.

---

## 10. Input — Keyboard and Mouse

Input is read once per frame inside `Window::run()` via `input->fetch()`. By the time `update()` is called, all states are ready.

### Keyboard

```cpp
Keyboard* kb = state->input->keyboard;

if (kb->w)     { /* W held */ }
if (kb->space) { /* Space held */ }
if (kb->f5)    { /* F5 held */ }
if (kb->alt && kb->enter) { window->toggleFullscreen(); }
```

Available key properties: `up`, `down`, `left`, `right`, `w`, `a`, `s`, `d`, `space`, `tab`, `f5`, `f11`, `f12`, `alt`, `enter`, `plus`, `minus`.

For arbitrary keys use the raw array:

```cpp
if (kb->keys[SDL_SCANCODE_Q]) { ... }
```

### Mouse

```cpp
Mouse* mouse = state->input->mouse;

mouse->x, mouse->y           // current position (screen coords)
mouse->leftClick             // true for one frame on click
mouse->rightClick            // true for one frame on right click
mouse->leftDragActive        // true while left button held and dragging
mouse->leftDragStartX/Y      // where the drag began
mouse->inside(&someRect)     // true if mouse is over the rect
```

To convert a mouse click to world coordinates:

```cpp
float worldX = state->camera->getX() + mouse->x;
float worldY = state->camera->getY() + mouse->y;
```

### Cooldown — Preventing Key Repeat

Use `Cooldown` to rate-limit actions that should not fire every frame:

```cpp
#include <game/cooldown.h>

Cooldown* cooldown = new Cooldown(200); // 200ms cooldown

if (kb->f5 && cooldown->isReady()) {
    generate();
    cooldown->reset();
}
```

---

## 11. Jobs — Task Queue System

The **Job** system gives units a queue of tasks to execute over time. Each `Job` runs its `update()` every frame until it signals completion.

### Built-in Job: Move

```cpp
#include <game/move.h>

// Queue a move toward a world offset
float travelX = targetWorldX - unit->getX();
float travelY = targetWorldY - unit->getY();
unit->addJob(new Move(unit, travelX, travelY));
```

The `Move` job advances the unit toward the destination each frame using `canMove()` for collision, and finishes when it arrives.

### Writing a Custom Job

```cpp
#include <game/job.h>

class AttackJob : public Job {
public:
    static constexpr int TYPE = 99;
    Unit* target;

    AttackJob(Unit* self, Unit* target) : Job(self), target(target) {}

    virtual bool update(State* state) override {
        // Return true to mark the job as complete
        float dist = /* distance to target */;
        if (dist < 10.0f) {
            // deal damage...
            return true; // done
        }
        // move closer...
        return false;
    }

    virtual int getType() override { return TYPE; }
};
```

### Job Queue API

```cpp
unit->addJob(new Move(unit, dx, dy));   // push a job
unit->removeJob(Move::TYPE);            // cancel all jobs of this type
unit->updateJobs(state);                // call once per frame in update()
```

### Job Completion Callback

Override `onJobFinished()` in your unit to react when a job ends:

```cpp
void MyUnit::onJobFinished(Job* job) {
    if (job->getType() == Move::TYPE) {
        action = IDLE;
        body->play(IDLE_DOWN);
    }
}
```

### Unique Jobs

If a job type should only ever have one instance at a time in the queue, override `isUnique()` to return `true`. Adding a second unique job of the same type removes the first.

---

## 12. Minimap

`Minimap` is a bird's-eye view of the entire map, rendered as a small overlay. It shows terrain colors and unit dots. The green rectangle (the "scope") shows the current camera viewport.

### Setup

```cpp
#include <game/minimap.h>

minimap = new Minimap(
    renderer,
    250, 250,      // minimap pixel size (width, height)
    500, 500,      // world grid size (must match Map)
    4              // scale: how many pixels per grid cell on the minimap
);

map->setMinimap(minimap);  // attach — map will keep terrain colors up to date
addObject(minimap);        // add to scene so it renders
```

### Toggle Visibility

```cpp
minimap->toggleVisible();  // hide/show with Tab key (from your update code)
```

### Drag to Navigate

The minimap supports left-click drag to pan the camera. This is handled automatically once the minimap is added to the scene.

### Unit Colors on the Minimap

Set a unit's minimap dot color in its constructor:

```cpp
setColor({255, 255, 0, 255}); // yellow dot for this unit
```

---

## 13. UI Primitives — Text, Rectangle, Select, FPS

### Text

```cpp
#include <game/text.h>

TTF_Font* font = TTF_OpenFont("assets/fonts/titillium.ttf", 20);
TTF_SetFontOutline(font, 1);

Text* label = new Text(font);
label->setText("Hello!");
label->setColor({255, 220, 0, 255});
label->setPositionFixed(true);  // screen-space coords (not affected by camera)
label->setPosition(10, 10);
addObject(label);

// Update text every frame:
label->setText("FPS: " + std::to_string((int)state->clock->fps));
```

Floating labels attached to a unit (using child positions):

```cpp
// In Unit constructor:
text = new Text(font, createChildPosition(-10, -40));
```

Call `text->render(state)` manually in the unit's `render()`.

### Rectangle

```cpp
#include <game/rectangle.h>

Rectangle* panel = new Rectangle(
    {255, 255, 255, 200},  // border color
    {0,   0,   0,  100}    // background color
);
panel->setPosition(10, 10);
panel->setSize(200, 80);
panel->show();
addObject(panel);
```

### Select — Drag-Box Selection

`Select` draws a rubber-band selection rectangle when the user left-drag clicks on the map.

```cpp
#include <game/select.h>

addObject(new Select(
    {255, 255, 255, 100},  // border color
    {1,   1,   1,  100}    // fill color
));
```

The box adjusts automatically based on mouse drag. Integrate it with your units by checking `mouse->leftDragEnded` and testing unit positions against the drag rectangle.

### FPS Counter

```cpp
#include <game/fps.h>

Fps* fps = (new Fps(font))->setPositionFixed(true);
fps->setColor({255, 220, 0, 255});
addObject(fps);
```

Toggle visibility:

```cpp
fps->toggleVisible();
```

---

## 14. Networking — WebSocket Client

GameWork includes a binary-protocol WebSocket client for real-time multiplayer.

### Defining a Message

Define a plain struct with a `static constexpr uint8_t type` field — that's all `Protocol` needs:

```cpp
struct UnitState {
    static constexpr uint8_t type = 1;  // unique message type byte
    uint32_t object_id;
    float    x;
    float    y;
    uint32_t action;
};
```

The `Protocol` class serializes it into a compact binary format: `[1 byte type][2 bytes size][N bytes payload]`.

### Connecting

```cpp
#include <game/client.h>

Client* client = new Client();
client->connect("ws://localhost:9000");
client->enableAutoReconnect(true);
state->client = client;  // store on state so units can access it
```

### Sending a Message

```cpp
UnitState msg {
    .object_id = getId(),
    .x         = getX(),
    .y         = getY(),
    .action    = (uint32_t)action,
};
state->client->send(msg);
```

### Receiving Messages

Register handlers in `prepare()`:

```cpp
client->setHandler<UnitState>([this](const UnitState& msg) {
    Object* obj = getObject(msg.object_id);
    if (!obj) return;
    MyUnit* unit = dynamic_cast<MyUnit*>(obj);
    if (unit) {
        unit->setPosition(msg.x, msg.y);
    }
});
```

### Polling

The client must be polled every frame (done automatically if you set `state->client`):

```cpp
if (state->client) state->client->poll();
```

---

## 15. Putting It All Together

This section shows how all the pieces connect in a typical game scene. The class names here are illustrative — replace them with whatever suits your game.

### Scene

```cpp
#pragma once
#include <game/scene.h>
#include <game/map.h>
#include <game/minimap.h>
#include <game/fps.h>
#include <game/cooldown.h>

#define GRID_W 500
#define GRID_H 500

class MyScene : public Scene {
    using Scene::Scene;
    Cooldown* cooldown;
    Fps*      fps;
public:
    virtual void prepare(State* state) override;
    virtual void update(State* state) override;
    void generateMap();
};
```

### prepare()

```cpp
void MyScene::prepare(State* state) {
    cooldown = new Cooldown(200);

    TTF_Font* font = TTF_OpenFont("assets/fonts/myfont.ttf", 20);
    TTF_SetFontOutline(font, 1);

    // 1. Build the map
    map = new Map(new Image(renderer, "assets/sprites/tileset.png"),
                  32, 32, GRID_W, GRID_H, 3, font);

    // 2. Build the minimap and attach it
    minimap = new Minimap(renderer, 250, 250, GRID_W, GRID_H, 4);
    map->setMinimap(minimap);

    // 3. Define terrain types
    map->terrains = {
        new Terrain(0, LAYER_WATER,  {51,  51,  255}),
        new Terrain(1, LAYER_GROUND, {102, 178, 215}),
        new Terrain(2, LAYER_GROUND, {255, 255, 255}),
    };

    // 4. Define tile transition rules (see section 8)
    map->tiles = { ... };

    // 5. Generate terrain
    generateMap();

    // 6. Load sprites (store in Scene::sprites to share between units)
    sprites[1] = (new Sprite(new Image(renderer, "assets/sprites/hero.png"), 72, 72, 100, true))
        ->addClip(MOVE_UP,   1, 2, 4)
        ->addClip(MOVE_DOWN, 5, 2, 4)
        ->addClip(IDLE_UP,   1, 1, 1)
        ->addClip(IDLE_DOWN, 5, 1, 1);

    // 7. Spawn units
    MyUnit* unit = new MyUnit(sprites[1], font);
    unit->setMap(map);
    if (unit->canOccupy(64, 64, 32, 32)) {
        unit->setPosition(64, 64);
        addObject(unit, unit->getId());
        unit->select();
    }

    // 8. UI
    addObject(new Select({255, 255, 255, 100}, {1, 1, 1, 100}));
    fps = (new Fps(font))->setPositionFixed(true);
    addObject(fps);

    // 9. Optional networking
    // client->connect("ws://localhost:9000");
    // client->setHandler<MyMessage>(...);
}
```

### update()

```cpp
void MyScene::update(State* state) {
    Keyboard* kb = state->input->keyboard;

    // Camera scroll
    if (kb->up)    state->camera->addY(-1);
    if (kb->down)  state->camera->addY(1);
    if (kb->left)  state->camera->addX(-1);
    if (kb->right) state->camera->addX(1);

    // Zoom
    if (kb->plus)  state->camera->zoomIn();
    if (kb->minus) state->camera->zoomOut();

    // Fullscreen
    if (kb->alt && kb->enter) window->toggleFullscreen();

    // Cooldown-gated hotkeys
    if (kb->f5  && cooldown->isReady()) { generateMap(); cooldown->reset(); }
    if (kb->f12 && cooldown->isReady()) { map->toggleDebug(); cooldown->reset(); }
    if (kb->tab && cooldown->isReady()) { minimap->toggleVisible(); cooldown->reset(); }

    Scene::update(state); // propagates to all registered objects
}
```

### A Unit with Movement and Jobs

```cpp
void MyUnit::update(State* state) {
    Keyboard* kb  = state->input->keyboard;
    Mouse*    m   = state->input->mouse;
    Camera*   cam = state->camera;

    float moveX = 0, moveY = 0;
    bool  isInput = false;

    if (kb->w) { moveY = -1; isInput = true; }
    if (kb->s) { moveY =  1; isInput = true; }
    if (kb->a) { moveX = -1; isInput = true; }
    if (kb->d) { moveX =  1; isInput = true; }

    if (isInput) {
        removeJob(Move::TYPE);
        action = move(state->clock->delta, moveX, moveY) ? MOVE : IDLE;
    } else {
        if (m->rightClick) {
            float tx = (cam->getX() + m->x) - getX();
            float ty = (cam->getY() + m->y) - getY();
            addJob(new Move(this, tx, ty));
            action = MOVE;
        }
        updateJobs(state);
        // Optional: camera follows this unit
        cam->setX(getX() + getWidth() / 2 - cam->getWidth() / 2);
        cam->setY(getY() + getHeight() / 2 - cam->getHeight() / 2);
    }

    body->play(action, (speed / 100) * 6.25f);
    body->update(state->clock->delta);
}

void MyUnit::onJobFinished(Job* job) {
    if (job->getType() == Move::TYPE) action = IDLE;
}

void MyUnit::render(State* state) {
    Camera* cam = state->camera;
    if (cam->isVisible(getRenderPosition())) {
        if (isSelected()) drawPosition(state);
        body->render(cam->translate(getRenderPosition()));
        label->render(state);
    }
}
```

---

## Architecture Overview

```
Window
└── run()
    ├── input->fetch()          // read SDL events
    ├── clock->tick()           // update delta / fps
    ├── scene->update(state)    // game logic
    │   ├── scene objects' update()
    │   └── unit->updateJobs()  // run job queue
    ├── scene->render(state)    // draw everything
    │   └── scene objects' render()
    └── client->poll()          // handle network messages
```

```
Object                  (src/game/object.h)
├── Unit                (terrain, layers, job queue, minimap color)
│   └── YourUnit        (extend Unit for every in-world entity)
├── Map                 (tile world)
├── Minimap             (bird's-eye overlay)
├── Text                (font rendering)
├── Rectangle           (colored box)
│   └── Select          (drag-selection box)
├── Fps                 (frame rate display)
└── Torch               (circular light effect)
```

---

## Common Keyboard Patterns

| Key | Action |
|-----|--------|
| WASD | Move selected unit |
| Arrow keys | Scroll camera |
| Right-click | Move unit to clicked position (pathfind) |
| + / - | Zoom in / out |
| Alt+Enter | Toggle fullscreen |
| F5 | Regenerate map |
| F11 | Toggle FPS counter |
| F12 | Toggle map debug overlay |
| Tab | Toggle minimap |
