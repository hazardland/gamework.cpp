# Phase 2: Simplify Position

## Goal

Make `Position` easy to understand at a glance.

Right now `Position` is doing too many things at once:

- raw rectangle storage
- parent-relative positioning
- parent-relative scaling
- cached recalculation
- debug drawing

The target for this phase is not to redesign the whole system.
The target is to make it smaller, clearer, and less surprising without breaking the current framework behavior.

## Scope

Files directly in scope:

- [src/game/position.h](/d:/src/gamework/src/game/position.h)
- [src/game/position.cpp](/d:/src/gamework/src/game/position.cpp)

Files likely affected:

- [src/game/object.h](/d:/src/gamework/src/game/object.h)
- [src/game/object.cpp](/d:/src/gamework/src/game/object.cpp)
- [src/game/unit.cpp](/d:/src/gamework/src/game/unit.cpp)
- [src/klad1/player.cpp](/d:/src/gamework/src/klad1/player.cpp)
- [src/klad1/bridge.cpp](/d:/src/gamework/src/klad1/bridge.cpp)

## Principles

For this phase:

- keep behavior unless behavior is obviously accidental
- prefer removing confusion over adding abstraction
- do not rename everything at once
- compile after each meaningful change

## What Is Wrong With Position Now

Current issues:

1. `Position` includes [state.h](/d:/src/gamework/src/game/state.h) in the header only because it has `draw(...)`
2. `draw(...)` makes geometry depend on rendering
3. `rect` is public, which weakens the abstraction
4. parent-relative logic is not easy to infer quickly
5. the file contains some naming/comments that reflect history more than clear current intent

## Desired End State For This Phase

After this phase, `Position` should:

- represent geometry and attachment only
- not render itself
- not include rendering/game state headers in the header
- keep current parent-relative behavior working
- be easier to read without chasing unrelated code

## Step 1: Remove Debug Drawing From Position

### Intent

`Position` should not know how to render debug rectangles.

### Work

1. Remove `draw(State* state)` declaration from [position.h](/d:/src/gamework/src/game/position.h)
2. Remove `draw(...)` implementation from [position.cpp](/d:/src/gamework/src/game/position.cpp)
3. Remove `#include <game/state.h>` from [position.h](/d:/src/gamework/src/game/position.h)
4. Remove [camera.h](/d:/src/gamework/src/game/camera.h) include from [position.cpp](/d:/src/gamework/src/game/position.cpp) if it becomes unused

### Why

This gives an immediate simplification:

- geometry no longer depends on renderer/state
- header becomes lighter
- class purpose becomes clearer

### Compile Check

Compile:

- `src/game/position.cpp`
- `src/game/object.cpp`
- `src/game/unit.cpp`

Then build:

- Klad1 target

### Exit Condition

`Position` no longer contains any rendering/debug behavior.

## Step 2: Make The Public Surface Smaller

### Intent

Reduce the amount of raw internal state exposed.

### Work

1. Review whether `rect` can become private
2. If current code still needs direct access:
- add/keep `getPosition()`
- add minimal specific accessors instead of exposing internals
3. Update call sites if needed

### Why

Public `rect` makes it harder to know whether `Position` is meant to be a real abstraction or just a struct.

### Caution

Do not force this if too many active files still depend on raw `rect`.
If needed, postpone full privacy and just document this as a later follow-up.

### Compile Check

Compile:

- `src/game/position.cpp`
- `src/game/object.cpp`
- `src/game/map.cpp` or later `world.cpp`
- `src/game/unit.cpp`
- `src/klad1/player.cpp`

Then build:

- Klad1 target

### Exit Condition

Either:

- `rect` is private

or:

- we have a clear note that privacy is postponed because active code still relies on it

## Step 3: Clarify Parent-Attached Modes

### Intent

Keep current relative behavior, but make it easier to understand.

### Work

1. Review constructor overloads
2. Make the two modes obvious:
- standalone position
- parent-attached position
3. Improve local naming in implementation if needed
- for example make parent-relative logic read more directly
4. Remove stale comments that explain old experiments rather than current behavior

### Why

This is the most confusing part of the file for a reader.

### Important Constraint

Do not redesign the math unless necessary in this phase.
The first priority is readability, not changing layout semantics.

### Compile Check

Compile:

- `src/game/position.cpp`
- `src/game/object.cpp`
- `src/game/unit.cpp`
- `src/klad1/player.cpp`
- `src/klad1/bridge.cpp`

Then build:

- Klad1 target

### Exit Condition

A reader can tell what each constructor mode is for without deep inference.

## Step 4: Clean Ready/Cache Behavior

### Intent

Make the cached recalculation logic easier to follow.

### Work

1. Review:
- `needsUpdate`
- `ready`
- `setRequiresUpdate()`
- `recalculateIfNeeded()`
2. Simplify naming if helpful
3. Remove any redundant state transitions
4. Keep the behavior that current `Object`/`Unit` code relies on

### Why

This is another place where the file feels more magical than it should.

### Compile Check

Compile:

- `src/game/position.cpp`
- `src/game/object.cpp`
- `src/game/unit.cpp`

Then build:

- Klad1 target

### Exit Condition

Cache/update flow is understandable without tracing every setter mentally.

## Step 5: Review Object Integration

### Intent

After simplifying `Position`, make sure `Object` still uses it cleanly.

### Work

1. Re-check [object.cpp](/d:/src/gamework/src/game/object.cpp)
2. Confirm:
- `setPosition`
- `addPosition`
- `setSize`
- child position creation
are still coherent
3. Make only small alignment edits here if needed

### Why

`Object` is the main consumer of `Position`, so Phase 2 is not complete unless that integration still reads well.

### Compile Check

Compile:

- `src/game/object.cpp`
- `src/game/unit.cpp`
- `src/game/scene.cpp`

Then build:

- Klad1 target

### Exit Condition

`Object` + `Position` still work together cleanly after the cleanup.

## Step 6: Final Phase 2 Review

### Checklist

Ask these questions:

1. Can `Position` be explained in one short sentence now?
2. Does it still mix rendering concerns into geometry?
3. Are constructor modes easier to tell apart?
4. Did we reduce header coupling?
5. Did we avoid changing behavior unnecessarily?

If any answer is bad, do one more small cleanup before ending the phase.

### Final Compile Check

Build:

- Klad1 target

Optional runtime smoke test:

- launch Klad1
- verify startup
- verify player still renders and moves

## Expected Deliverable After Phase 2

After this phase, we should have:

- a smaller `Position`
- cleaner header dependencies
- no rendering logic inside geometry
- no behavior regressions in Klad1

This phase does not aim to finish all `Position` redesign.
It aims to make the file understandable and less surprising without destabilizing the framework.
