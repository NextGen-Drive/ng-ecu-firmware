#include "../animations/NightModeTransition.hpp"

struct NightModeChangedEvent
{
    static void handle(bool newValue)
    {
        if (newValue)
        {
            if (!currentAnimation->isCompleted || IS_NIGHT_MODE)
                return;
            Log::println("Starting night mode animation");
            delete currentAnimation;
            IS_NIGHT_MODE = true;
            currentAnimation = new NightModeTransition();
            currentAnimation->startAnimation();
        }
        else
        {
            if (!currentAnimation->isCompleted || !IS_NIGHT_MODE)
                return;
            Log::println("Starting day mode animation");
            IS_NIGHT_MODE = false;
            delete currentAnimation;
            currentAnimation = new NightModeTransition();
            currentAnimation->startAnimation();
        }
    }
};