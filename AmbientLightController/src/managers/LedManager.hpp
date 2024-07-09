#pragma once

#include <themes/ITheme.hpp>
#include <themes/ChillTheme.hpp>
#include <animations/Animation.hpp>
#include <FastLED.h>

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
        FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS);
        FastLED.addLeds<WS2811, DATA_PIN_CHARGING_STATION_LIGHT>(charging_station_leds, NUM_LEDS);
        FastLED.addLeds<WS2811, DATA_PIN_DOOR_FR_SPEAKER>(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER);
        FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FR_POCKET, GRB>(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET);
        FastLED.addLeds<WS2812B, DASH_BOTTOM_PIN, GRB>(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS);
        FastLED.addLeds<WS2811, DATA_PIN_DOOR_FL_SPEAKER>(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER);
        FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FL_POCKET, GRB>(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET);

        FastLED.clear();
        FastLED.setBrightness(255);
    }

    static void tick()
    {
        currentAnimation->tickAnimation();

        FastLED.show();
    }
};

bool LedManager::IS_NIGHT_MODE = true;
ITheme *LedManager::CurrentTheme = new ChillTheme();
Animation *LedManager::currentAnimation = nullptr;