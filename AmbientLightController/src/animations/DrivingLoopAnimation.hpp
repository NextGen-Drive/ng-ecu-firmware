#pragma once

#include "managers/LedManager.hpp"
#include <FastLED.h>

constexpr uint16_t LOOP_DURATION_MS = 20000; // 20s loop
constexpr uint16_t NUM_WAVES = 3;
constexpr uint16_t STAR_COUNT = 5;

class DrivingLoopAnimation : public Animation
{
private:
    unsigned long loopStart = 0;
    unsigned long lastUpdateTime = 0;
    bool isFirstFrame = true;

    struct Wave
    {
        float speed;
        int direction;
        CRGB color;
        float phaseOffset;
    } waves[NUM_WAVES];

    struct Star
    {
        int index;
        uint8_t brightness;
        unsigned long nextBlink;
    } stars[STAR_COUNT];

public:
    void startAnimation() override
    {
        loopStart = millis();
        lastUpdateTime = loopStart;
        isFirstFrame = true;

        waves[0] = {1.0f, 1, CRGB(180, 240, 255), 0.0f};
        waves[1] = {1.4f, -1, CRGB(0, 255, 200), 0.3f};
        waves[2] = {1.8f, 1, CRGB(100, 180, 255), 0.6f};

        for (int i = 0; i < STAR_COUNT; i++)
        {
            stars[i].index = random(0, DASH_BOTTOM_NUM_LEDS);
            stars[i].brightness = random(10, 25);
            stars[i].nextBlink = millis() + random(3000, 8000);
        }
    }

    void tickAnimation() override
    {
        unsigned long now = millis();
        unsigned long elapsed = now - loopStart;
        float baseT = fmodf((float)elapsed / LOOP_DURATION_MS, 1.0f);
        float breathing = 0.25f + 0.2f * sinf(TWO_PI * baseT);

        bool isNight = LedManager::IS_NIGHT_MODE;
        uint8_t dashMax = isNight ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS;

        // Animate only dash
        for (int i = 0; i < DASH_BOTTOM_NUM_LEDS; i++)
        {
            float pos = (float)i / DASH_BOTTOM_NUM_LEDS;

            CRGB baseColor = blend(CRGB(255, 255, 255), CRGB(0, 255, 180), pos * 255);

            float brightnessMod = 1.0f;

            for (int w = 0; w < NUM_WAVES; w++)
            {
                float localT = fmodf(baseT * waves[w].speed + waves[w].phaseOffset, 1.0f);
                float wavePos = waves[w].direction == 1 ? localT : (1.0f - localT);
                float dist = fabs(wavePos - pos);
                float influence = expf(-dist * 30.0f);
                brightnessMod += influence * 1.2f; // etwas stärker
                baseColor = blend(baseColor, waves[w].color, influence * 150);
            }

            brightnessMod = constrain(brightnessMod, 0.0f, 2.0f);
            LedRegistry::dash_bottom_leds[i] = adjustBrightness(baseColor, breathing * dashMax * brightnessMod);
        }

        if (isFirstFrame)
        {
            isFirstFrame = false;

            uint8_t pocketMax = isNight ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS;
            uint8_t speakerMax = isNight ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS;
            uint8_t chargerMax = isNight ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS;

            CRGB staticColor = CRGB(160, 220, 255); // leicht bläulich

            fill_solid(LedRegistry::door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, adjustBrightness(staticColor, pocketMax));
            fill_solid(LedRegistry::door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, adjustBrightness(staticColor, pocketMax));

            fill_solid(LedRegistry::door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, adjustBrightness(staticColor, speakerMax));
            fill_solid(LedRegistry::door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, adjustBrightness(staticColor, speakerMax));

            fill_solid(LedRegistry::charging_station_leds, NUM_LEDS, adjustBrightness(staticColor, chargerMax));
        }

        lastUpdateTime = now;
    }
};
