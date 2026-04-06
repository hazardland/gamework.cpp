// #include <iostream>

#include "game/input.h"
#include "game/terrain.h"
#include "game/image.h"
#include "game/protocol.h"
#include "game/state.h"
#include "game/torch.h"

#include "examples/war2.h"
#include "examples/enum.h"
#include "examples/footman.h"
#include "examples/farm.h"
#include "examples/critter.h"
#include "examples/tree.h"
#include "examples/messages.h"

// Prepare function
void Warcraft::prepare(State* state) {

    cooldown = new Cooldown(200);

    printf("Preparing scene...\n");


    font = TTF_OpenFont("assets/fonts/titillium.ttf", 20);
    TTF_SetFontOutline(font, 1);
    fontSmall = TTF_OpenFont("assets/fonts/titillium.ttf", 12);
    TTF_SetFontOutline(fontSmall, 1);
    fps = (new Fps(font))->setPositionFixed(true);
    fps->setColor({255, 220, 0, 255});

    printf("Creating map\n");

    map = new Map(
        new Image(renderer, "assets/sprites/winter.png"),
        32, 32,
        WIDTH, HEIGHT, 3,
        fontSmall
    );

    printf("Creating minimap\n");

    minimap = new Minimap(
        renderer,
        250, 250,
        WIDTH, HEIGHT, 4
    );

    printf("Setting minimap\n");

    map->setMinimap(minimap);

    // map->setDebug(true);
    // map->debug = false;
    // objects.insert({map->getId(), map});

    printf("Cofiguring tiles and terrains\n");

    map->terrains = {
        // Water
        new Terrain(TERRAIN_WATER, LAYER_WATER, {51, 51, 255}),
        // Ice
        new Terrain(TERRAIN_SHORE, LAYER_GROUND, {102, 178, 215}),
        // Ground
        new Terrain(TERRAIN_GROUND, LAYER_GROUND, {255, 255, 255}),
        // Forest
        new Terrain(TERRAIN_FOREST, LAYER_GROUND, {34, 139, 34}) // Dark green for forests
    };

    map->tiles = {

        {   0, {319, 320, 321, 319, 320, 321, 319, 320, 321, 319, 320, 321 , 322, 323, 324}}, // Base water
        {1111, {331, 332, 333, 334, 331, 332, 333, 334, 331, 332, 333, 334, 331, 332, 333, 334, 331, 332, 333, 334 , 335, 336, 337, 338, 339}},
        { 111, {199, 200}}, // Ice crossing water
        {1011, {201, 202}},
        {  11, {203, 204, 205}},
        {1101, {206, 207}},
        { 101, {208, 209, 210}},
        {   1, {212, 213}},
        {1110, {214, 215}},
        {1010, {217, 218, 219}},
        {  10, {220, 221}},
        {1100, {222, 223, 224}},
        { 100, {225, 226}},
        {1000, {227, 228}},
        {1001, {174}}, //271
        {2222, {349, 350, 351, 349, 350, 351, 349, 350, 351, 349, 350, 351 , 352, 353, 354, 355, 356, 357, 358, 359, 363}}, // Base ground
        {1222, {259, 260}}, // Ground crossing ice
        {2122, {261, 262}},
        {1122, {263, 264, 265}},
        {2212, {266, 267}},
        {1212, {268, 269, 270}},
        {1112, {273, 274}},
        {2221, {275, 276}},
        {2121, {279, 280, 281}},
        {1121, {282, 283}},
        {2211, {284, 285, 286}},
        {1211, {287, 288}},
        {2111, {289, 290}},
        {2112, {174}}, //271
        {1221, {278}}, //271,
        {3333, {108, 109, 111, 112, 113, 114, 119, 120}}, // Base forest
        {2333, {108, 109, 111, 112, 113, 114, 119, 120}}, // {129, 104}}, // Forest crossing ground
        {3233, {108, 109, 111, 112, 113, 114, 119, 120}}, // {107, 131}},
        {2233, {106, 130}},
        {3323, {108, 109, 111, 112, 113, 114, 119, 120}}, // {102, 127}},
        {2323, {115, 117, 128}},
        {2223, {129}},
        {3332, {108, 109, 111, 112, 113, 114, 119, 120}}, // {133, 110}},
        {3232, {116}}, // {115, 118}},
        {2232, {131}},
        {3322, {124, 134}},
        {2322, {102}},
        {3222, {110}},
        {3223, {379}}, //372
        {2332, {379}}, //372,

    };


    generate();


    sprites[SPRITE_FOOTMAN_RED] = (new Sprite(new Image(renderer, "assets/sprites/grunt.png"),
                                72,
                                72,
                                100,
                                true))
    ->addClip (MOVE+UP, 1, 2, 4, false, false)
    ->addClip (MOVE+DOWN, 5, 2, 4, false, false)
    ->addClip (MOVE+RIGHT, 3, 2, 4, false, false)
    ->addClip (MOVE+LEFT, 3, 2, 4, true, false)
    ->addClip (MOVE+UP+RIGHT, 2, 2, 4, false, false)
    ->addClip (MOVE+DOWN+RIGHT, 4, 2, 4, false, false)
    ->addClip (MOVE+UP+LEFT, 2, 2, 4, true, false)
    ->addClip (MOVE+DOWN+LEFT, 4, 1, 4, true, false)
    ->addClip (IDLE+UP, 1, 1, 1, false, false)
    ->addClip (IDLE+DOWN, 5, 1, 1, false, false)
    ->addClip (IDLE+RIGHT, 3, 1, 1, false, false)
    ->addClip (IDLE+LEFT, 3, 1, 1, true, false)
    ->addClip (IDLE+UP+RIGHT, 2, 1, 1, false, false)
    ->addClip (IDLE+DOWN+RIGHT, 4, 1, 1, false, false)
    ->addClip (IDLE+UP+LEFT, 2, 1, 1, true, false)
    ->addClip (IDLE+DOWN+LEFT, 4, 1, 1, true, false)
    ->addClip (ATTACK+UP, 1, 6, 4, false, false)
    ->addClip (ATTACK+DOWN, 5, 6, 4, false, false)
    ->addClip (ATTACK+RIGHT, 3, 6, 4, false, false)
    ->addClip (ATTACK+LEFT, 3, 6, 4, true, false)
    ->addClip (ATTACK+UP+RIGHT, 2, 6, 4, false, false)
    ->addClip (ATTACK+DOWN+RIGHT, 4, 6, 4, false, false)
    ->addClip (ATTACK+UP+LEFT, 2, 6, 4, true, false)
    ->addClip (ATTACK+DOWN+LEFT, 4, 6, 4, true, false);

    sprites[SPRITE_HUMAN_FARM] = (new Sprite(new Image(renderer, "assets/sprites/farm.png"),
                                64,
                                64,
                                100,
                                true))->addClip();


    sprites[SPRITE_HUMAN_MACDONALDS] = (new Sprite(new Image(renderer, "assets/sprites/chicken.png"),
    96,
    96,
    100,
    true))->addClip();


    sprites[SPRITE_TREE1] = (new Sprite(new Image(renderer, "assets/sprites/pine1.png"),
    64,
    96,
    100,
    true))->addClip();

    sprites[SPRITE_TREE2] = (new Sprite(new Image(renderer, "assets/sprites/pine2.png"),
    64,
    96,
    100,
    true))->addClip();

    sprites[SPRITE_CRITTER] = (new Sprite(new Image(renderer, "assets/sprites/critter.png"),
    32,
    32))
    ->addClip(UP, 1, 1, 1, false, false)
    ->addClip(UP+RIGHT, 2, 1, 1, false, false)
    ->addClip(UP+LEFT, 2, 1, 1, true, false)
    ->addClip(RIGHT, 3, 1, 1, false, false)
    ->addClip(LEFT, 3, 1, 1, true, false)
    ->addClip(RIGHT+DOWN, 4, 1, 1, false, false)
    ->addClip(LEFT+DOWN, 4, 1, 1, true, false)
    ->addClip(DOWN, 5, 1, 1, false, false);

    printf("Creating farms\n");
    for (int x=1000; x<10*128; x+=128) {
        for (int y=128; y<10*128; y+=128) {
            Farm* farm = new Farm(sprites[SPRITE_HUMAN_MACDONALDS]);
            farm->setMap(map);
            if (farm->canPlace(x-20, y-20, 64+20, 64+20)) {
                farm->setPosition(x, y);
                addObject(farm);
            }

        }
    }

    printf("Creating trees\n");
    srand(time(NULL));

    
    // footmanDebugText = (new Text(fontSmall))
    //                     ->enableCache()
    //                     ->setPositionFixed(false);

    // printf("Creating critters\n");
    // for (int x=500; x<500+100*32; x+=32) {
    //     for (int y=500; y<500+100*32; y+=32) {
    //         Critter* critter = new Critter(sprites[SPRITE_CRITTER], font);
    //         critter->setMap(map);
    //         if (critter->canPlace(x, y, 32, 32)) {
    //             critter->setPosition(x, y);
    //             addObject(critter, critter->getId());
    //         }
    //     }
    // }


    printf("Creating footmans\n");
    Footman* lastFootman = NULL;
    for (int x=0; x<1*32; x+=32) {
        for (int y=10; y<1*32; y+=32) {
            Footman* footman = new Footman(sprites[SPRITE_FOOTMAN_RED], fontSmall);
            footman->setMap(map);
            if (footman->canPlace(x, y, 32, 32)) {
                footman->setPosition(x, y);
                addObject(footman, footman->getId());
                lastFootman = footman;
            }
        }
    }
    if (lastFootman!=nullptr) {
        lastFootman->select();
    }

    printf("Creating trees\n");   
    // Loop in Y → X order to ensure back-to-front drawing
    for (int y = 128; y < 10 * 96; y += 36) { // Reduced spacing for overlap
        for (int x = 228; x < 10 * 64; x += 32) {
            if (rand() % 100 < 80) { // 80% chance to spawn a tree
                int offsetX = rand() % 12 - 6;  // Slight horizontal jitter
                int offsetY = rand() % 6;       // Tiny vertical offset
                int treeType = (rand() % 100 < 60) ? SPRITE_TREE1 : SPRITE_TREE2; // Mix them
    
                Tree* tree = new Tree(sprites[treeType]);
                tree->setMap(map);
                int treeX = x + offsetX;
                int treeY = y + offsetY;
    
                // if (tree->canPlace(treeX, treeY, 64, 96)) {
                    tree->setPosition(treeX, treeY);
                    addObject(tree); // assume draw order is by Y
                // }
            }
        }
    }

    // addObject(new Torch({0, 0, 0, 255},  100, 320));
        
    printf("Creating fps\n");

    // objects.insert({minimap->getId(), minimap});
    // objects.insert({fps->getId(), fps});
    
    addObject(new Select({255,255,255,100},{1, 1, 1, 100}));
    
    // client = new Client();
    if (client!=nullptr) {
        printf("Waiting for connection to server\n");
        printf("But first server should be running did you forget to run server executable? -_-\n");
        client->connect("ws://localhost:9000");
        client->enableAutoReconnect(true);
        client->setHandler<FootmanState>([this](const FootmanState& footmanState) {
            Object* obj = getObject(footmanState.object_id);
            if (!obj) return;
            
            Footman* footman = dynamic_cast<Footman*>(obj);
            if (footman) {
                footman->setPosition(footmanState.x, footmanState.y);
                footman->sync(
                    footmanState.action, 
                    footmanState.faceX,
                    footmanState.faceY
                );
            }
        });
        state->client = client;
    }
    
    addObject(fps);
}

// Generate function
void Warcraft::generate() {
    // srand(clock());
    srand(2657);
    int seed = rand();
    printf("Map seed: %d\n", seed);
    map->generate(seed, 0.05, {0.2, 0.45, 0.8, 1});
    // map->import(
    //     {
    //         {0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {1,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {1,1,1,1,0,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0},
    //         {0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0},
    //         {0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0},
    //         {1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0},
    //         {1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,0},
    //         {0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0,0,0,0},
    //         {0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    //     }
    // );
}

// Update function
void Warcraft::update(State* state) {

    Keyboard* keyboard = state->input->keyboard;

    if (keyboard->plus) {
        state->camera->zoomIn();
    }
    if (keyboard->minus) {
        state->camera->zoomOut();
    }

    if (keyboard->alt && keyboard->enter) {
        window->toggleFullscreen();
    }

    if (keyboard->down) {
        state->camera->addY(1);
    }
    if (keyboard->up) {
        state->camera->addY(-1);
    }
    if (keyboard->right) {
        state->camera->addX(1);
    }
    if (keyboard->left) {
        state->camera->addX(-1);
    }

    if (keyboard->f5 && cooldown->isReady()) {
        generate();
        cooldown->reset();
    }
    if (keyboard->f12 && cooldown->isReady()) {
        map->toggleDebug();
        cooldown->reset();
    }
    if (keyboard->f11 && cooldown->isReady()) {
        fps->toggleVisible();
        cooldown->reset();
    }
    if (keyboard->tab && cooldown->isReady()) {
        minimap->toggleVisible();
        cooldown->reset();
    }

    Scene::update(state);

}

// /**

// "1.1.1.x" = {0},
// "1.1.x.x" = {1},
// "1.1.x.1" = {2},
// "1.x.1.x" = {28},
// "x.1.x.1" = {30},
// "1.x.1.1" = {56},
// "x.x.1.1" = {57},
// "x.1.1.1" = {58},
// "x.x.x.1" = {3},
// "x.x.1.x" = {4},
// "x.1.x.x" = {31},
// "1.x.x.x" = {32},
// "x.1.1.x" = {59},
// "1.x.x.1" = {60},
// "1.1.1.1" = {5, 6, 33, 34}

// "2.2.2.x" = {84},
// "2.2.x.x" = {85},
// "2.2.x.2" = {86},
// "2.x.2.x" = {112},
// "x.2.x.2" = {114},
// "2.x.2.2" = {140},
// "x.x.2.2" = {141},
// "x.2.2.2" = {142},
// "x.x.x.2" = {87},
// "x.x.2.x" = {88},
// "x.2.x.x" = {115},
// "2.x.x.x" = {116},
// "x.2.2.x" = {143},
// "2.x.x.2" = {144},
// "2.2.2.2" = {89, 117}

// "0.0.0.1" = {168},
// "0.0.1.1" = {169},
// "0.0.1.0" = {170},
// "0.1.0.1" = {196},
// "1.0.1.0" = {198},
// "0.1.0.0" = {224},
// "1.1.0.0" = {225},
// "1.0.0.0" = {226},
// "1.1.1.0" = {171},
// "1.1.0.1" = {172},
// "1.0.1.1" = {199},
// "0.1.1.1" = {200},
// "1.0.0.1" = {227},
// "0.1.1.0" = {228},
// "0.0.0.0" = {173, 174, 201, 202, 229}


// */
