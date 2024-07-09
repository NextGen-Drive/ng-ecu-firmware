#pragma once

#include "../animations/NightModeTransition.hpp"
#include "managers/LedManager.hpp"

struct NightModeChangedEvent
{
    static void handle(bool newValue)
    {
        if (newValue)
        {
            if (!LedManager::currentAnimation->isCompleted || LedManager::IS_NIGHT_MODE)
                return;

            Log::println("Starting night mode animation");
            delete LedManager::currentAnimation;
            LedManager::IS_NIGHT_MODE = true;
            LedManager::currentAnimation = new NightModeTransition();
            LedManager::currentAnimation->startAnimation();
        }
        else
        {
            if (!LedManager::currentAnimation->isCompleted || !LedManager::IS_NIGHT_MODE)
                return;

            Log::println("Starting day mode animation");
            LedManager::IS_NIGHT_MODE = false;
            delete LedManager::currentAnimation;
            LedManager::currentAnimation = new NightModeTransition();
            LedManager::currentAnimation->startAnimation();
        }
    }
};