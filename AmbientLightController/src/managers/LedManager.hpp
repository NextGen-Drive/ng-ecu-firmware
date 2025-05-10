#pragma once

#include <themes/ITheme.hpp>
#include <themes/ChillTheme.hpp>
#include <animations/Animation.hpp>
#include <FastLED.h>
#include "LedRegistry.hpp"

#define DATA_PIN 8
#define DATA_PIN_CHARGING_STATION_LIGHT 12
#define NUM_LED_MIDDLE 16

#define DATA_PIN_DOOR_FR 4

struct LedManager
{
public:
    static bool IS_NIGHT_MODE;
    static ITheme *CurrentTheme;
    static Animation *currentAnimation;

    static void init()
    {
        Log::println("LedManager: Initializing!");

        FastLED.clear();
        FastLED.setBrightness(255);
        
        Log::println("LedManager: Initialized!");
    }

    static void tick()
    {
        if (currentAnimation) {
            currentAnimation->tickAnimation();
        }

        FastLED.show();
    }
};

bool LedManager::IS_NIGHT_MODE = true;
ITheme *LedManager::CurrentTheme = new ChillTheme();
Animation *LedManager::currentAnimation = nullptr;