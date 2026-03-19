Controls
-------------------
+ Fix diagonal movment (Shrink if diagal movement detected both vectors with 40%)
+ Alt + Enter = Toggle Fullscreen
+ Character movement A W S D
+ Arrow Keys - Scroll camera with cooldown
+ Camera zoom cooldown config
. Restore cameraFollow from git commit history
. Mouse wheel scroll zoom
. Left mouse drag scroll
- F5 - Regenerate (Lacks object destroy and recreate)
. Select units with selectbox

UI
------------------------
. FontCache
. Input?

Architecture
-----------------------
. Make private things private
. Proper object and unit destroy 
. Footman class naming brainstorm
+ Map - Use integer tile keys in the map instead of strings
+ Investigate minimap further optimization
. SDL2 from pacman? No need for extra includes, latest version
. SDL3?
+ Unit.addPosition with updateGrid()
. Hide grid and make methods for setting and getting


Big Challanges
----------------------
. Job system - Introduce job sysem for automated tasks
    . Make some simple move job
. LUA - Try lua scripting
. If those work can I update objects in another thread?
. Find nearest possible enemy and call attack on it like receiveDamage
. We need good renderer which will maintain order from top to bottom

Network
--------------------------
+ Network - Implement network thread
+ Should aggregate messages in the socket before sending
. Strange bug with many units 10000k
. I do not like client in state
. main --server=ws://localhost:9000 - For websocket
. Server as authority with a map - client as replayer of sandbox
    Separate server
. Mask Position mode, modeX, modeY name it better

LUA
--------------------------
. Investigate LUA
. Lua nice single file module for Scene for Footman for Farm etc

Features
-------------------------
. Cool text logger, a log is an array of text messages, they fade away
    we insert in log a text message simply like log(string) and it does a thing
. Remove SDL_FRect mention from all code and wrap it without FPS sacrifase



Big ideas
---------------
. Multisection grids for infinite worlds
. Make sandbox were animals live plants grow and eath each other

