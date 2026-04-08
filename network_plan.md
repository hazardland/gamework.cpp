# Multiplayer Plan

## Goal

Build the smallest useful multiplayer slice first:

- run `klad1.exe` on two laptops
- see two players
- moving one player on one machine moves the mirrored player on the other
- keep sprite state correct enough for idle / run / climb / fall
- keep the whole feature easy to disable with one line

This first version is not the final server-authoritative architecture.
It is the smallest version that proves:

- networking works
- protocol is understandable
- player sync feels good enough
- the code can later evolve into a real game

## First Version Scope

Start with a dumb relay model:

- each client runs the game normally
- one local player is controlled by input
- one remote player is updated from network messages
- a relay server just forwards messages between clients

This is intentionally smaller than:

- server-authoritative world simulation
- move jobs
- rollback
- prediction
- reconciliation

Those can come later.

## Why Start This Way

The main risk is not sockets.
The main risk is making player sync fight the existing movement and animation code.

So first we reuse current `Player` behavior as much as possible and only add:

- a way to publish local player state
- a way to apply remote player state

That gets us to a visible result quickly.

## Minimal Mental Model

Keep the networking story this small:

1. `Protocol`
   encodes and decodes tiny packed messages

2. `Client`
   connects, polls, sends typed messages, receives typed messages

3. `KladNet`
   glues one local `Player` and one remote `Player` to the `Client`

4. relay server
   forwards binary messages to other connected clients

This should be explainable in one line:

`KladNet mirrors one local player to remote clients through typed WebSocket messages.`

## First Message Type

Create one game-specific message in `src/klad1/messages.h`.

Suggested first version:

```cpp
#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct PlayerState {
    static constexpr uint8_t type = 1;
    uint32_t id;
    float x;
    float y;
    float moveX;
    float moveY;
    float falling;
};
#pragma pack(pop)
```

This is enough to reconstruct:

- idle
- run
- climb
- fall

without inventing a bigger animation protocol yet.

If needed later, add:

- `uint8_t action`
- `bool flipX`

But do not start there unless the existing player animation cannot be reconstructed cleanly.

## Files To Create

### `src/klad1/messages.h`

Purpose:

- define Klad1-specific network messages
- keep them out of `src/game`

Initial contents:

- `PlayerState`

### `src/klad1/net.h`

Purpose:

- tiny glue layer between `Client` and `Player`
- keep networking out of `Player` and mostly out of `Klad1`

Suggested shape:

```cpp
#pragma once

#include <string>

class Client;
class Player;

class KladNet {
public:
    KladNet(Player* localPlayer, Player* remotePlayer);
    ~KladNet();

    bool connect(const std::string& uri);
    void update();

private:
    Client* client = nullptr;
    Player* localPlayer = nullptr;
    Player* remotePlayer = nullptr;
    uint32_t localId = 1;
    uint32_t remoteId = 2;
};
```

### `src/klad1/net.cpp`

Purpose:

- own the `Client`
- set handlers
- send local player state
- apply remote player state

No game logic beyond player mirroring.

## Files To Modify

### `src/klad1/player.h`

Add the minimum needed to support remote sync.

Suggested additions:

- `bool isRemote = false;`
- method to apply network state

Example:

```cpp
void syncRemote(float x, float y, float moveX, float moveY, float falling);
void setRemote(bool remote);
```

### `src/klad1/player.cpp`

Keep existing local behavior intact.

For remote player behavior:

- remote player does not read keyboard input
- remote player updates from stored network state
- sprite/animation state should still be chosen by the existing logic as much as possible

Possible first approach:

- store remote target state
- in `update()`:
  - if `isRemote`, skip input
  - use synced movement values to choose clips
  - optionally lerp position a little toward target

Simplest first version:

- direct snap to received `x` and `y`

If it looks bad:

- add a tiny smoothing step later

### `src/klad1/klad1.h`

Add:

- `KladNet* net = nullptr;`
- maybe `Player* localPlayer`
- maybe `Player* remotePlayer`

### `src/klad1/klad1.cpp`

In `prepare()`:

- create local player
- create remote player
- mark remote player as remote
- optionally create `KladNet`

Suggested enable point:

```cpp
net = new KladNet(localPlayer, remotePlayer);
net->connect("ws://192.168.1.10:9000");
```

This should stay easy to disable:

```cpp
// net = new KladNet(localPlayer, remotePlayer);
// net->connect("ws://192.168.1.10:9000");
```

In `update()`:

- if `net != nullptr`, call `net->update()`

## Disable Story

The feature should be easy to turn off without removing code.

The desired shape is:

- comment one or two lines in `Klad1::prepare()`
- maybe leave `net` as `nullptr`
- game still works as single-player

That is important because this is an experiment layer first, not a permanent hard dependency.

## Suggested `KladNet` Flow

### On connect

- create `Client`
- connect to URI
- register `PlayerState` handler

### Each frame

1. `client->poll()`
2. read local player state
3. `client->send(PlayerState{...})`
4. optionally `client->flush()` if current design needs explicit flush

### On receive

- if message is for local player id, ignore
- otherwise apply to remote player

## `Client` API Review

Current `Client` is close to useful already.

Keep this as the public story:

- `connect(uri)`
- `poll()`
- `send(message)`
- `setHandler<T>(handler)`
- `isConnected()`
- `close()`

These are easy to understand and enough for the first version.

### Public API To De-emphasize

These may stay implemented, but they should not be central to the basic usage story:

- `wait()`
- `flush()`
- reconnect complexity

For now, think of them as advanced details.

### `Client` Simplification Ideas

Later cleanup ideas:

1. Add a short top comment in `client.h` showing intended use:

```cpp
// Client client;
// client.connect("ws://localhost:9000");
// client.setHandler<PlayerState>(...);
// client.send(message);
// client.poll();
```

2. Make the first-usage path obvious.

3. Consider whether `flush()` should become internal later if the non-blocking send path can be made self-explanatory.

4. Consider renaming `setHandler` later only if a better name becomes obvious.
   Right now it is acceptable.

## `Protocol` API Review

`Protocol` is already pretty good.

Keep it small:

- `encode`
- `decode`
- `type`
- `size`

That is enough.

### `Protocol` Simplification Ideas

1. Tighten comments so they focus on:
   - packed POD messages
   - `static constexpr uint8_t type`
   - encode / decode use

2. Keep game-specific message structs out of `src/game`.
   The framework only provides the binary wrapper.

3. Later, if desired, add a short note that messages should be:
   - packed
   - stable
   - simple value types

No redesign needed right now.

## Remote Player Behavior

The remote player should be a normal `Player` with one extra rule:

- it does not consume local input

That means:

- same sprite
- same render logic
- same animation selection logic if possible

This is important because it avoids inventing a second character type too early.

## Smoothness

Do not overbuild smoothness in the first pass.

Start with:

- send every frame or every 30-50 ms
- apply state directly

If motion looks too jumpy:

- store `targetX` and `targetY`
- move a fraction toward them each frame

That is enough for LAN testing.

## Relay Server Plan

Do not keep the current root `server.cpp` as framework design.
Treat it as an experiment.

For the first multiplayer demo, the server only needs to:

- accept WebSocket clients
- relay binary packets to the other clients

That is enough for a working mirror demo.

### Later Server Goal

After the first demo works, move toward:

- authoritative server
- headless `World`
- clients send requests
- server validates and broadcasts accepted state

This is the real long-term direction, but not the first slice.

## Long-Term Authoritative Direction

Target shape later:

- server owns headless `World`
- clients send input or move requests
- server checks `canPlace` / world logic
- server updates unit state
- server broadcasts resulting state or movement events

First version can still broadcast positions.

Later versions can broadcast:

- move start
- move stop
- climb start
- state snapshots

Do not start there.

## Concrete Phase Plan

### Phase 1: Mirror Demo

Goal:

- two Klad1 instances
- one local player
- one remote player
- local movement mirrored remotely

Work:

- add `messages.h`
- add `net.h/.cpp`
- add remote player support in `Player`
- enable from `Klad1`

### Phase 2: Clean Story

Goal:

- tiny docs for `Client` and `Protocol`
- simplify usage comments
- keep multiplayer easy to disable

Work:

- small comment cleanup in `client.h`
- small comment cleanup in `protocol.h`
- maybe add a short `network.md` usage note later

### Phase 3: Real Server

Goal:

- separate relay or authoritative server into its own clear file or folder

Work:

- replace experimental `server.cpp`
- choose one server model:
  - dumb relay first
  - authoritative world later

### Phase 4: Authoritative World

Goal:

- move from state mirroring to real validated gameplay

Work:

- headless `World`
- request messages
- accepted state messages

## Opinion Summary

Recommended first move:

- do not archive `client.*`
- do not archive `protocol.h`
- do not design full server authority yet
- first get a tiny mirrored-player demo working with a relay

That gets us to something visible and reviewable fast.

After that, we can move toward the real dream:

- headless authoritative world
- validated multiplayer
- still simple and understandable
