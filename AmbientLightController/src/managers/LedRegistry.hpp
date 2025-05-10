#pragma once

#include <FastLED.h>

#define DATA_PIN 8
#define DATA_PIN_CHARGING_STATION_LIGHT 12
#define NUM_LED_MIDDLE 16

#define LED_REGS_LENGTH 8

#define DATA_PIN_DOOR_FR 4

enum LedType
{
    Unknown,
    DOOR_SPEAKER,
    DOOR_POCKET,
    PHONE_CHARGER,
    DASHBOARD,
};

struct LedRegistration
{
    CRGB *leds;
    uint64_t length;
    LedType type;

    LedRegistration()
    {
    }

    LedRegistration(CRGB *leds, uint64_t length, LedType type)
    {
        this->leds = leds;
        this->length = length;
        this->type = type;
    }
};

struct LedRegistry
{
public:
    //static LedRegistration regs[LED_REGS_LENGTH];

    static CRGB charging_station_leds[NUM_LEDS];
    static CRGB door_fr_leds[NUM_LEDS_DOOR_FR];
    static CRGB door_fr_leds_speaker[NUM_LEDS_DOOR_FR_SPEAKER];
    static CRGB door_fr_leds_pocket[NUM_LEDS_DOOR_FR_POCKET];
    static CRGB dash_bottom_leds[DASH_BOTTOM_NUM_LEDS];
    static CRGB door_fl_leds_speaker[NUM_LEDS_DOOR_FL_SPEAKER];
    static CRGB door_fl_leds_pocket[NUM_LEDS_DOOR_FL_POCKET];

    static void init()
    {
        Log::println("LedRegistry: Initializing!");
        
        FastLED.addLeds<WS2811, DATA_PIN_CHARGING_STATION_LIGHT>(charging_station_leds, NUM_LEDS);
        onRegister(charging_station_leds, NUM_LEDS, PHONE_CHARGER);

        FastLED.addLeds<WS2811, DATA_PIN_DOOR_FR_SPEAKER>(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER);
        onRegister(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, DOOR_SPEAKER);

        FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FR_POCKET, GRB>(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET);
        onRegister(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, DOOR_POCKET);

        FastLED.addLeds<WS2812B, DASH_BOTTOM_PIN, GRB>(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS);
        onRegister(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, DASHBOARD);

        FastLED.addLeds<WS2811, DATA_PIN_DOOR_FL_SPEAKER>(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER);
        onRegister(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, DOOR_SPEAKER);

        FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FL_POCKET, GRB>(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET);
        onRegister(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, DOOR_POCKET);

        Log::println("LedRegistry: Initialized!");
    }

private:
    static uint16_t ledRegIndex;

    static void onRegister(CRGB *leds, uint64_t length, LedType type)
    {
        //regs[ledRegIndex] = LedRegistration(leds, length, type);
        ledRegIndex++;

        Log::println("Registered LED ID: " + String(ledRegIndex));
    }
};

//LedRegistration LedRegistry::regs[LED_REGS_LENGTH] = {};
CRGB LedRegistry::charging_station_leds[NUM_LEDS];
CRGB LedRegistry::door_fr_leds[NUM_LEDS_DOOR_FR];
CRGB LedRegistry::door_fr_leds_speaker[NUM_LEDS_DOOR_FR_SPEAKER];
CRGB LedRegistry::door_fr_leds_pocket[NUM_LEDS_DOOR_FR_POCKET];
CRGB LedRegistry::dash_bottom_leds[DASH_BOTTOM_NUM_LEDS];
CRGB LedRegistry::door_fl_leds_speaker[NUM_LEDS_DOOR_FL_SPEAKER];
CRGB LedRegistry::door_fl_leds_pocket[NUM_LEDS_DOOR_FL_POCKET];
uint16_t LedRegistry::ledRegIndex = 0;