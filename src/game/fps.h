#ifndef GAME_FPS_H
#define GAME_FPS_H

#include <SDL3/SDL_timer.h>

#include "game/text.h"
#include <game/cooldown.h>

class Context;

/**
 * @class Fps
 * @brief A class that extends the Text class to provide a FPS (Frames Per Second) counter display.
 *
 * The FPS display is updated every half a second to show the current FPS and Delta time.
 * The position of this display is always at the top right corner of the screen.
 */
class Fps: public Text {
public:
    using Text::Text;  ///< Inherit the constructors of the Text class.
    Cooldown* cooldown = new Cooldown(500);

    /// Destructor
    virtual ~Fps() = default;

    /**
     * @brief Updates the FPS and Delta time display.
     *
     * The FPS and Delta time are updated every 500ms.
     *
     * @param state The current game state.
     */
    void update(Context* context) override;

    /**
     * @brief Returns whether the FPS counter is visible.
     *
     * The FPS counter is always visible.
     *
     * @param state The current game state.
     * @return True because FPS counter is always visible.
     */
    bool isVisible(Context* context) override;

private:
    int ticks = 0;  ///< Time in milliseconds when the FPS and Delta time were last updated.
};

#endif // GAME_FPS_H


