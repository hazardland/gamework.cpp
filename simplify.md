# Framework Simplification Plan

## Goal

Make the framework:

- intuitive
- short
- easy to scan in context
- easy to remember after time away
- simple in core, but still sharp in functionality

This plan assumes:

- active target is Klad1
- War2 is archived reference only
- old examples are not compatibility gates

## Core Design Rules

These are the rules the framework should move toward.

### 1. Keep only core concepts

Core should contain only things that most games need:

- `Object`
- `Unit`
- `World` (currently still `Map`)
- `Cell`
- `Scene`
- `Window`
- `State`
- `Input`
- `Mouse`
- `Keyboard`
- `Camera`
- rendering helpers like `Image`, `Sprite`, `Clip`, `Animation`

### 2. Keep concepts separate

- `Object`
  generic scene thing
- `Unit`
  world-tracked thing with collision and movement
- `Cell`
  stores data
- `World`
  stores cells and unit buckets

### 3. Keep names short and literal

Prefer:

- `tile`
- `type`
- `canPlace`
- `canCrossTile`
- `canCrossUnit`
- `World`

Avoid:

- names that hide two meanings
- names that need extra explanation every time
- old words kept only because the code evolved around them

### 4. Layers stay, but only for unit collision

Layers are still useful because they let the world search only inside the relevant bucket.

Layer should mean only:

- which units collide with me

Layer should not mean:

- tile passability
- render order
- special wildcard terrain behavior

### 5. Optional systems leave the core

These do not belong in the small framework core:

- minimap
- generation
- autotile filling
- networking
- RTS-specific selection helpers

They can exist later as optional modules.

## Current Status

Already simplified:

- `Cell` now uses `tile` and `type`
- active path no longer depends on `Terrain`
- active path no longer depends on minimap coupling
- `Unit` uses:
  - `canPlace(...)`
  - `canCrossTile(...)`
  - `canCrossUnit(...)`
- active Klad1 build compiles with the simplified core

Still worth simplifying:

- `Input`
- `Mouse`
- `Keyboard`
- `Position`
- `Object`
- `Window`
- `State`
- `Sprite` / `Clip` / `Animation`
- `Map` rename to `World`
- optional modules moved out of core thinking

## Compile Rule

After every phase:

1. compile changed framework files
2. compile changed Klad1 files
3. build full Klad1 target
4. fix breakage before next phase

Use this rule strictly.

## Recommended Compile Checks

### Fast file checks

Use direct compile for touched files, for example:

```powershell
g++ -std=c++20 -O3 -m64 -Isrc -s $(pkg-config --cflags sdl3 sdl3-image sdl3-ttf) -c src/game/input.cpp -o build/src/game/input.o
```

### Full active build

Use the Klad1 build after each meaningful phase:

```powershell
make klad1
```

If `make` is unreliable in the shell, use the PowerShell-safe full build path already used during refactor.

### Runtime smoke test

After bigger phases:

- launch Klad1
- confirm startup
- confirm player can still move
- confirm world still renders

## Phase 1: Clean Input Surface

### Goal

Make input handling easy to scan at a glance.

### Files

- [src/game/input.h](/d:/src/gamework/src/game/input.h)
- [src/game/input.cpp](/d:/src/gamework/src/game/input.cpp)
- [src/game/mouse.h](/d:/src/gamework/src/game/mouse.h)
- [src/game/mouse.cpp](/d:/src/gamework/src/game/mouse.cpp)
- [src/game/keyboard.h](/d:/src/gamework/src/game/keyboard.h)
- [src/game/keyboard.cpp](/d:/src/gamework/src/game/keyboard.cpp)

### Work

1. Remove dead comments and debug traces from `Input`
2. Move drag/click transitions into `Mouse`
3. Move scancode-to-bool filling into `Keyboard`
4. Make `Input::fetch()` only:
- reset devices
- poll SDL events
- hand off to mouse/keyboard

### Expected Result

`Input::fetch()` should fit on one screen and read as one simple flow.

### Compile Check

- compile `src/game/input.cpp`
- compile `src/game/mouse.cpp`
- compile `src/game/keyboard.cpp`
- build Klad1

### Exit Condition

No big policy blob remains inside `Input::fetch()`.

## Phase 2: Simplify Position

### Goal

Make position logic understandable without reading hidden math twice.

### Files

- [src/game/position.h](/d:/src/gamework/src/game/position.h)
- [src/game/position.cpp](/d:/src/gamework/src/game/position.cpp)

### Work

1. Remove rendering/debug behavior from `Position`
2. Remove unnecessary includes from the header
3. Make the core role obvious:
- raw rect storage
- optional parent-relative attachment
4. Decide whether parent-relative scaling stays here or becomes a helper
5. Reduce surprising public surface where possible

### Expected Result

`Position` should read like geometry, not geometry plus UI/debug/layout logic.

### Compile Check

- compile `src/game/position.cpp`
- compile `src/game/object.cpp`
- compile `src/game/unit.cpp`
- build Klad1

### Exit Condition

A reader can understand `Position` quickly without chasing unrelated behavior.

## Phase 3: Thin Object

### Goal

Keep `Object` boring and obvious.

### Files

- [src/game/object.h](/d:/src/gamework/src/game/object.h)
- [src/game/object.cpp](/d:/src/gamework/src/game/object.cpp)

### Work

1. Review whether lazy id allocation is still needed
2. Keep only the generic object responsibilities
3. Reduce wrapper noise if `Position` got simpler
4. Keep child positions only if they are still clearly justified

### Expected Result

`Object` should be easy to explain in one sentence:

"A generic scene object with position, update, render, and visibility hooks."

### Compile Check

- compile `src/game/object.cpp`
- compile `src/game/unit.cpp`
- compile `src/game/scene.cpp`
- build Klad1

### Exit Condition

`Object` no longer feels clever.

## Phase 4: Clean Window And Startup

### Goal

Make app startup and loop simple to follow.

### Files

- [src/game/window.h](/d:/src/gamework/src/game/window.h)
- [src/game/window.cpp](/d:/src/gamework/src/game/window.cpp)
- [src/game/state.h](/d:/src/gamework/src/game/state.h)
- [src/game/state.cpp](/d:/src/gamework/src/game/state.cpp)

### Work

1. Remove leftover debug prints
2. Remove duplicate includes
3. Make ownership and cleanup clearer
4. Keep `Window` focused on:
- SDL window/runtime init
- scene hookup
- main loop
5. Keep `State` as a simple runtime bundle

### Expected Result

A new reader should be able to find:
- where the app starts
- where the scene is attached
- where the frame loop runs

with almost no searching.

### Compile Check

- compile `src/game/window.cpp`
- compile `src/game/state.cpp`
- compile `klad1_main.cpp`
- build Klad1

### Exit Condition

Startup path becomes obvious.

## Phase 5: Clean Rendering Ownership

### Goal

Make the sprite/animation stack predictable.

### Files

- [src/game/image.h](/d:/src/gamework/src/game/image.h)
- [src/game/image.cpp](/d:/src/gamework/src/game/image.cpp)
- [src/game/sprite.h](/d:/src/gamework/src/game/sprite.h)
- [src/game/sprite.cpp](/d:/src/gamework/src/game/sprite.cpp)
- [src/game/clip.h](/d:/src/gamework/src/game/clip.h)
- [src/game/clip.cpp](/d:/src/gamework/src/game/clip.cpp)
- [src/game/animation.h](/d:/src/gamework/src/game/animation.h)
- [src/game/animation.cpp](/d:/src/gamework/src/game/animation.cpp)

### Work

1. Make ownership explicit
2. Make lookups safer
3. Reduce duplicate state where possible
4. Remove stale debug or dead paths

### Expected Result

It should be easy to answer:

- who owns the image?
- who owns the clips?
- how does animation pick a frame?

without guessing.

### Compile Check

- compile all changed render files
- compile `src/klad1/player.cpp`
- compile `src/klad1/klad1.cpp`
- build Klad1

### Exit Condition

Rendering ownership is obvious.

## Phase 6: Rename Map To World

### Goal

Use the clearer name now that the class is simple enough.

### Files

- [src/game/map.h](/d:/src/gamework/src/game/map.h)
- [src/game/map.cpp](/d:/src/gamework/src/game/map.cpp)
- all active includes and references in `src/game` and `src/klad1`

### Work

1. Rename `Map` to `World`
2. Rename files:
- `map.h` -> `world.h`
- `map.cpp` -> `world.cpp`
3. Update include paths and types
4. Keep method names short and literal

### Expected Result

The class name matches what it really is now:

- a world grid
- cell store
- tile renderer
- unit bucket manager

### Compile Check

- compile changed framework files
- compile changed Klad1 files
- build Klad1

### Exit Condition

No active code should still conceptually treat it as the old heavy `Map`.

## Phase 7: Move Optional Systems Out Of Core Thinking

### Goal

Reduce framework mental load.

### Files

Likely:

- [src/game/job.h](/d:/src/gamework/src/game/job.h)
- [src/game/move.h](/d:/src/gamework/src/game/move.h)
- [src/game/move.cpp](/d:/src/gamework/src/game/move.cpp)
- [src/game/client.h](/d:/src/gamework/src/game/client.h)
- related client implementation files

### Work

1. Decide which optional systems remain useful
2. If kept, treat them as optional modules
3. Remove debug prints and stale code from them
4. Do not let them define the shape of the core framework

### Expected Result

The core framework can be understood without first understanding:
- jobs
- networking
- RTS-like systems

### Compile Check

- compile changed optional files
- build Klad1

### Exit Condition

Core and optional systems are mentally separate.

## Phase 8: Final Scan For LLM Readability

### Goal

Make the framework easy to read in a small context window.

### Work

For each active core file, ask:

1. Can the class be explained in one short sentence?
2. Does the filename match the real responsibility?
3. Are there comments that explain history instead of behavior?
4. Are there dead code blocks or stale commented code?
5. Are there hidden ownership rules?
6. Are there names that cause double-guessing?

Clean whatever still fails those checks.

### Expected Result

A small set of files where each one has one clear role.

### Compile Check

- full Klad1 build
- quick runtime smoke test

### Exit Condition

Framework is easy for both you and an LLM to scan quickly.

## Naming Guidance

Keep names:

- short
- literal
- stable
- single-purpose

Prefer:

- `World`
- `Cell`
- `tile`
- `type`
- `Object`
- `Unit`
- `canPlace`
- `canCrossTile`
- `canCrossUnit`

Avoid:

- abstract words that hide policy
- long names with redundant context
- names that need explanation in conversation

## What To Discard

Discard from core:

- minimap
- terrain objects
- generation
- autotile filling
- legacy reference compatibility
- debug trace leftovers

## What To Protect

Protect these ideas:

- object/unit split
- per-layer unit buckets
- world-based collision
- type/tile split
- drag behavior in mouse handling
- simple explicit APIs

## Execution Rule

For every phase:

1. make one coherent change
2. compile touched files
3. build Klad1
4. fix breakage
5. only then move on

Do not batch multiple conceptual refactors into one step.
