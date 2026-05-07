# Map Usage

This note documents how the current `src/game/map.*` system is used in the repo today.

It is based on the working setup in [war2.cpp](/d:/src/gamework/src/examples/war2.cpp).

## What `Map` Currently Does

The current `Map` class combines several responsibilities:

- stores a grid of `Cell*`
- stores a `Terrain*` for each cell
- chooses tile sprites based on neighboring terrain
- renders the visible part of the map
- supports grid-based occupancy and collision for `Unit`
- optionally pushes color updates into `Minimap`

Because of that, the current `Map` is closer to an RTS map system than a fully generic engine map.

## Constructor

Current constructor:

```cpp
Map(
    Image* image,
    int cellWidth,
    int cellHeight,
    int gridWidth,
    int gridHeight,
    int layerCount = 3,
    TTF_Font* font = NULL
)
```

Meaning:

- `image`: tileset image used for map rendering
- `cellWidth`, `cellHeight`: size of one map cell in world pixels
- `gridWidth`, `gridHeight`: number of cells in the map
- `layerCount`: number of occupancy layers stored in each cell
- `font`: only used for debug rendering

Example:

```cpp
map = new Map(
    new Image(renderer, "assets/war2/sprites/winter.png"),
    32, 32,
    WIDTH, HEIGHT,
    3,
    fontSmall
);
```

## Basic Setup Flow

The current setup flow is:

1. Create the map
2. Optionally create and attach a minimap
3. Define terrain types
4. Define tile transition rules
5. Fill the grid using `generate(...)` or `import(...)`

## Step 1: Create the Map

```cpp
map = new Map(
    new Image(renderer, "assets/war2/sprites/winter.png"),
    32, 32,
    WIDTH, HEIGHT,
    3,
    fontSmall
);
```

## Step 2: Optional Minimap

If the game needs a minimap:

```cpp
minimap = new Minimap(
    renderer,
    250, 250,
    WIDTH, HEIGHT,
    4
);

map->setMinimap(minimap);
```

If the game does not need a minimap, skip this step.

## Step 3: Define Terrains

`Map::terrains` is a list of terrain types indexed by terrain id.

Example:

```cpp
map->terrains = {
    new Terrain(TERRAIN_WATER,  LAYER_WATER,  {51, 51, 255}),
    new Terrain(TERRAIN_SHORE,  LAYER_GROUND, {102, 178, 215}),
    new Terrain(TERRAIN_GROUND, LAYER_GROUND, {255, 255, 255}),
    new Terrain(TERRAIN_FOREST, LAYER_GROUND, {34, 139, 34})
};
```

Each terrain currently contains:

- `id`
- `layer`
- `color`

In the current code, terrain affects:

- map rendering transitions
- unit movement permission
- minimap color

## Step 4: Define Tile Rules

`Map::tiles` maps a 4-corner terrain signature to one or more tile indices inside the tileset.

Example:

```cpp
map->tiles = {
    {0,    {319, 320, 321}},
    {1111, {331, 332, 333}},
    {2222, {349, 350, 351}},
    {3333, {108, 109, 111}}
};
```

Notes:

- the key is built from 4 corner terrain values
- the value is a list of candidate tile indices
- `Map` picks one randomly from that list

This is what drives the Warcraft-style terrain blending.

## Step 5A: Procedural Generation

To generate terrain from noise:

```cpp
int seed = rand();
map->generate(seed, 0.05f, {0.2f, 0.45f, 0.8f, 1.0f});
```

Meaning:

- `seed`: noise seed
- `intensity`: noise scale
- `ranges`: thresholds that choose which terrain id each sample becomes

The current `generate(...)` fills the terrain grid and then calls `fillMap()`.

## Step 5B: Manual Import

To import a map from terrain ids:

```cpp
map->import({
    {0, 0, 1, 1},
    {0, 1, 1, 2},
    {1, 1, 2, 2},
    {2, 2, 3, 3}
});
```

Notes:

- rows are indexed as `data[y][x]`
- each value is a terrain id
- `import(...)` also ends by calling `fillMap()`

## Rendering

The base scene currently renders the map with:

```cpp
map->render(state);
```

`Map::render(...)`:

- computes the visible cell region from the camera
- translates each visible cell to screen space
- draws the tile rect from the tileset image

## Unit / Collision Usage

RTS units use the map like this:

```cpp
footman->setMap(map);

if (footman->canOccupy(x, y, 32, 32)) {
    footman->setPosition(x, y);
}
```

Relevant unit behavior:

- `canOccupy(...)` checks map bounds
- checks terrain layer / allowed terrains
- checks collisions against other units in occupied cells
- `updateGrid()` stores the unit in the cells it currently occupies

So today, `Map` is part of the collision and occupancy system, not just rendering.

## Minimal Current Example

```cpp
Map* map = new Map(
    new Image(renderer, "assets/war2/sprites/winter.png"),
    32, 32,
    500, 500,
    3,
    fontSmall
);

map->terrains = {
    new Terrain(0, 0, {51, 51, 255}),
    new Terrain(1, 1, {102, 178, 215}),
    new Terrain(2, 1, {255, 255, 255}),
    new Terrain(3, 1, {34, 139, 34})
};

map->tiles = {
    {0, {319, 320, 321}},
    {1111, {331, 332, 333}},
    {2222, {349, 350, 351}},
    {3333, {108, 109, 111}}
};

map->generate(2657, 0.05f, {0.2f, 0.45f, 0.8f, 1.0f});
```

## For a Non-RTS Game

If you want to reuse `Map` for a non-RTS game such as `klad1`, the parts most likely still useful are:

- grid size
- tile rendering
- terrain ids
- manual import or procedural generation
- world-to-cell logic

The parts that are RTS-specific are:

- `Minimap`
- unit occupancy lists per cell layer
- terrain permission masks on `Unit`
- Warcraft-style terrain transition rules

That means `klad1` can still use the current map system, but probably only part of it should remain in the future cleaned-up framework.
