# Framework Patch Log

This file documents the safety and API cleanup patches applied to the `src/game` framework layer during the current pass.

## Applied Changes

### `src/game/window.cpp`

- Fixed `Window` construction when a `State*` is provided.
- `this->state` is now assigned whether the caller injects a state or not.
- Fixed the fullscreen display mode null check so `SDL_DisplayMode` is only dereferenced when SDL returns a valid pointer.

Reason:
- The previous constructor could dereference an uninitialized `this->state`.
- The previous fullscreen branch could dereference a null `displayMode`.

### `src/game/unit.h`

- Added default initialization for:
  - `moved`
  - `map`
  - `minimap`
  - `scene`
  - `renderPosition`
  - `selectPosition`

Reason:
- `Unit` is a base class for game entities such as `Footman`.
- These members previously started with indeterminate values until a derived constructor assigned them.
- `Unit::~Unit()` deletes `renderPosition` and `selectPosition`, so null-safe initialization is required.

### `src/game/object.h`

- Made `Object`'s destructor virtual.

Reason:
- `Object` is a polymorphic base class.
- Deleting derived instances through `Object*` must dispatch the correct destructor chain.

### `src/game/object.cpp`

- `Object::~Object()` now deletes the owned `position`.

Reason:
- `Object::Object()` allocates `position` with `new Position()`.
- That allocation previously leaked.

### `src/game/minimap.cpp`

- Stopped replacing the inherited `position` pointer with a second allocation.
- `Minimap` now uses `setPosition(0, 0)` and `setSize(...)` to configure the `Position` already created by `Object`.
- Added guards in `update()` and `render()` so the minimap does nothing until grid data and textures are valid.
- Added `Minimap::~Minimap()` cleanup for:
  - `drag`
  - `cooldown`
  - `background`
  - `foreground`

Reason:
- Replacing `position` conflicted with the base ownership model and leaked the original `Object` position.
- The minimap could read incomplete state before `setMapData()` ran.
- SDL textures and helper objects were allocated without matching cleanup.

### `src/game/minimap.h`

- Added safe default initialization for:
  - `grid`
  - `cellWidth`
  - `cellHeight`
  - `drag`
  - `cooldown`
  - `prevCameraX`
  - `prevCameraY`
  - `prevCameraWidth`
  - `prevCameraHeight`
- Declared `~Minimap() override`.

Reason:
- The minimap was reading cached camera values before initialization on first update.
- Pointer fields should start from a known null state.

### `src/game/scene.h`

- Added the missing `#include <unordered_map>`.

Reason:
- `Scene` directly uses `std::unordered_map` and should include the owning standard header itself.

### `src/game/scene.cpp`

- Fixed `Scene::addObject(Object* obj, uint32_t id)` to store the passed `id` instead of ignoring it and using `obj->getId()`.

Reason:
- The overload accepted an explicit ID but did not honor it.
- That could break object lookup for externally assigned IDs such as networked object identities.

## Verification

Attempted full rebuild:

```powershell
make clean; make
```

Result:
- Verification was not completed in the agent environment because Git's Windows `sh/rm` helper crashed with a signal-pipe error before a normal compile completed.
- The failure looked environment-related rather than tied to a specific C++ compile diagnostic.

## Remaining Work

These larger tasks are still intentionally separate from this patch pass:

- define clear ownership for `Scene`, `Window`, and `State`
- separate reusable framework code from game-specific systems such as `Map`, `Unit`, and `Minimap`
- review the websocket reconnect path in `src/game/client.cpp`
- reduce prototype-specific coupling inside `src/game`
