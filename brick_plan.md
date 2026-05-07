# Brick And Bullet Plan

## Goal

Implement Klad1 bricks so that:

- bullets damage bricks in 25% sections
- bullets visually travel through already damaged brick space
- bricks block the player until fully destroyed
- fully destroyed bricks regenerate one stage every 6 seconds
- the model is clean enough that later behavior changes do not break everything

This is one of the most sensitive gameplay systems, so the logic should be correct first and the rendering should follow the logic, not the other way around.

## Core Rules

### Brick Damage

Each brick has 4 virtual sections:

```text
[I|I|I|I]
```

Where:

- `I` = intact
- `D` = damaged / transparent to bullets

Each section represents 25% of brick width.

### Damage From Right

If a bullet comes from the right:

```text
[I|I|I|I] -> [I|I|I|D] -> [I|I|D|D] -> [I|D|D|D] -> [D|D|D|D]
```

### Damage From Left

If a bullet comes from the left:

```text
[I|I|I|I] -> [D|I|I|I] -> [D|D|I|I] -> [D|D|D|I] -> [D|D|D|D]
```

### Important Bullet Rule

Bullets pass through damaged sections.

That means:

- if 1 quarter is damaged, bullet passes through 25% of brick width
- if 2 quarters are damaged, bullet passes through 50% of brick width
- if 3 quarters are damaged, bullet passes through 75% of brick width
- bullet stops only at the first intact section

This is the most important interaction rule.

### Important Player Rule

Player collision is different from bullet collision:

- player is blocked while brick is not fully destroyed
- player can cross only when brick is fully destroyed
- regenerating brick is crossable until fully restored

So:

- bullets care about per-quarter damage
- player cares only whether the brick still has any intact section

## Brick State Model

Brick should store logical state, not just a sprite id.

Recommended fields:

```cpp
int damagedLeft = 0;
int damagedRight = 0;
Uint64 destroyedAt = 0;
bool regenerating = false;
```

Derived values:

```cpp
int getDamagedCount() const;
int getIntactCount() const;
bool isDestroyed() const;
bool isSolid() const;
```

Definitions:

- `getDamagedCount() = damagedLeft + damagedRight`
- `getIntactCount() = 4 - getDamagedCount()`
- `isDestroyed() = getIntactCount() == 0`
- `isSolid() = getIntactCount() > 0`

## Mixed Damage States

The gameplay rules allow mixed states, for example:

```text
[D|I|I|D]
[D|D|I|D]
[D|I|D|D]
```

Current art only supports:

- left-only damage:
  - `12`
  - `13`
  - `14`
- right-only damage:
  - `18`
  - `19`
  - `20`

There are no sprites for mixed partial states.

So the logic must support mixed states even if the initial rendering does not.

This is why logic should not be based on sprite ids.

### Current Decision

We are not fully covering mixed states yet.

Current asset decision:

- `[D|I|I|D]` now exists as a real appended atlas tile
- `[D|D|I|D]` temporarily renders as blank
- `[D|I|D|D]` temporarily renders as blank

The new mixed tile was built from:

- left half of tile `12`
- right half of tile `18`

and appended to the atlas as:

- `TILE_BRICK_DIID = 46`

This is intentionally temporary. Logic should still support all mixed states, even if the rendering falls back for two of them.

## Recommended Brick API

Suggested methods:

```cpp
enum class HitSide {
    Left,
    Right
};

class Brick : public Unit {
public:
    bool hit(HitSide side, float& hitX);
    bool isSolid() const;
    bool isDestroyed() const;
    int getSpriteTile() const;
    int getRegenTile() const;
    float getQuarterWidth() const;
    float getHitX(HitSide side) const;

    void update(Context* context) override;
    void render(Context* context) override;
    int getType() const override;
    bool canCrossUnit(Unit* target) const override;
};
```

Meaning:

- `hit(...)`
  applies one section of damage from left or right
- `hitX`
  returns where the bullet should stop visually
- `isSolid()`
  tells whether player should be blocked
- `update(...)`
  handles regeneration
- `render(...)`
  handles damage/regeneration visuals

## Bullet Interaction Model

Current bullet behavior is too simple:

- move
- overlap
- die

That is not enough for quarter-damaged bricks.

Instead, bullet should:

1. compute horizontal travel for this frame
2. search for the earliest hit in that direction
3. possible hits:
   - wall tile
   - brick intact section
4. stop at the earliest hit point
5. if brick hit:
   - damage that section
   - die
6. if wall hit:
   - die

## Bullet Vs Brick Hit Geometry

Let:

```cpp
float quarter = brickWidth / 4.0f;
```

### Bullet Coming From Left

The first intact section starts after damaged-left sections:

```cpp
hitX = brickX + damagedLeft * quarter;
```

That is the left edge of the first intact quarter.

### Bullet Coming From Right

The first intact section from the right starts here:

```cpp
hitX = brickX + brickWidth - damagedRight * quarter;
```

More precisely, the bullet should stop at the edge of the first intact quarter from that side.

So the bullet should not stop at brick bounds blindly. It should stop at the first intact quarter edge.

## Regeneration Model

Regeneration starts only when the brick becomes fully destroyed.

Recommended behavior:

- on the 4th damage hit:
  - brick becomes destroyed
  - set `regenerating = true`
  - store `destroyedAt = now`

Then regeneration stage advances every 6 seconds.

### Stage Timing

```text
0-6s   blank
6-12s  TILE_BRICK_REGEN_1 = 15
12-18s TILE_BRICK_REGEN_2 = 16
18-24s TILE_BRICK_REGEN_3 = 17
24s+   full brick
```

This is the clean implementation of:

- every 6 second next regen stage

Derived stage:

```cpp
int stage = elapsedMs / 6000;
```

Clamped to:

- `0` blank
- `1` regen 1
- `2` regen 2
- `3` regen 3
- `4` full brick

### On Full Regeneration

At full regeneration:

- `damagedLeft = 0`
- `damagedRight = 0`
- `regenerating = false`

and the brick becomes solid again.

## Rendering Plan

There are 2 possible rendering strategies.

### Option 1: Sprite Approximation

Use:

- `8` full brick
- `12,13,14` left-only damage
- `18,19,20` right-only damage
- `46` for `[D|I|I|D]`
- `15,16,17` regen
- blank when fully destroyed

Problem:

- two mixed states still do not have sprites:
  - `[D|D|I|D]`
  - `[D|I|D|D]`

So this option is incomplete unless mixed states are forbidden or visually approximated.

### Option 2: Quarter Rendering

Render brick by quarters:

- 4 vertical slices
- each slice either intact or empty
- use normal brick art for intact quarters
- overlay regen sprites when needed

This matches gameplay much better and supports mixed states naturally.

Recommended long-term choice:

- logic always uses quarter state
- rendering can start simple and later move to quarter rendering

## Recommended Implementation Order

### Phase 1: Brick Logic Only

Implement on `Brick`:

- quarter damage state
- `hit(...)`
- `isSolid()`
- regeneration timing

No fancy rendering yet.

### Phase 2: Bullet Hit Logic

Change bullet update so it:

- detects wall hit
- detects brick quarter hit
- stops at the correct hit point
- damages brick
- dies

This is the first important gameplay milestone.

### Phase 3: Player Collision

Make sure:

- partially damaged brick still blocks player
- fully destroyed / regenerating brick is crossable
- fully regenerated brick blocks again

### Phase 4: Rendering

Start with simplest useful visuals:

- full brick `8`
- left-only damage `12,13,14`
- right-only damage `18,19,20`
- `[D|I|I|D]` uses `46`
- `[D|D|I|D]` uses blank temporarily
- `[D|I|D|D]` uses blank temporarily
- blank when destroyed
- regen `15,16,17`

If mixed states look wrong, move to quarter rendering.

## New Findings

### Atlas

Current Klad1 atlas now includes one additional mixed brick frame:

- `46` = `[D|I|I|D]`

It was appended after the existing atlas content, so no older frame indices changed.

### Mixed-State Rendering Policy For Now

Temporary policy:

- `[D|I|I|D]` -> render `46`
- `[D|D|I|D]` -> render blank
- `[D|I|D|D]` -> render blank

This is only a rendering fallback. The logical damage state must still remain correct.

## Open Questions

### 1. Bullet And Mixed Partial States

Do we accept the temporary visual lie for:

- `[D|D|I|D]`
- `[D|I|D|D]`

while keeping gameplay correct?

Current assumption:

- yes

### 2. Regeneration Visual For Mixed States

When a mixed damaged brick is fully destroyed and starts regenerating, do we care about preserving the old damage-side visual history?

Current assumption:

- no
- once destroyed, regeneration uses only:
  - blank
  - `15`
  - `16`
  - `17`
  - full brick

### 3. Bullet Hit Resolution Against Brick Vs Wall

Implementation still needs a precise rule for “earliest hit wins” when a bullet could hit:

- a wall tile
- a brick intact quarter

in the same frame.

Current assumption:

- compare horizontal hit distance this frame
- whichever comes first wins

### 4. Brick Collision API Shape

The plan still should use:

- query hit point first
- mutate brick second

Recommended final shape remains:

- `float getHitX(HitSide side) const`
- `void hit(HitSide side)`

instead of one method that both computes and mutates.

## Important Design Notes

### 1. Do Not Use Sprite Id As Brick State

Sprite should be derived from state, not be the state.

Bad:

- `tile == 13 means brick has this logic`

Good:

- `damagedLeft = 2`
- `damagedRight = 0`
- render chooses sprite `13`

### 2. Bullet Collision Is Not Normal Unit Collision

Bullet must test:

- travel path
- first intact quarter
- not just final overlap

So bullet-brick interaction should be treated as a custom gameplay hit.

### 3. Player Collision And Bullet Collision Must Stay Separate

Player:

- blocked until fully destroyed

Bullet:

- passes through damaged quarters immediately

If those two ideas get mixed into one collision rule, the system will become confusing.

## Suggested First Coding Target

The first good checkpoint is:

- bullet damages brick quarter by quarter
- bullet dies on wall or brick
- player is still blocked until brick is fully destroyed
- fully destroyed brick becomes blank and starts regenerating

That gives a working gameplay loop before advanced rendering polish.
