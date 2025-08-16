#pragma once

#include "managers/LedManager.hpp"
#include "LogManager.hpp"
#include "animations/DrivingLoopAnimation.hpp"

constexpr uint16_t STAGE2_DURATION = 1000; // in ms
constexpr uint16_t STAGE1_DURATION = 1500;
constexpr uint16_t STAGE3_DURATION = 2000;
constexpr uint16_t STAGE4_DURATION = 2000;
constexpr uint16_t STAGE5_BLEND_DURATION = 500;

static inline float cubicEaseInOut(float t)
{
  return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3) / 2.0f;
}

static inline CRGB applyNightBrightness(CRGB color, uint8_t brightnessDay, uint8_t brightnessNight, bool isNight)
{
  uint8_t brightness = isNight ? brightnessNight : brightnessDay;
  return color.nscale8_video(brightness);
}

class StartupAnimation : public Animation
{
private:
  SharedAnimationParams dpCommon{0, 0, 0, 0, CRGB::Black};

  struct
  {
    uint8_t stage = 0;
    bool isDone = false;
    int16_t currentLEDPos = 0;
    uint16_t stageStartTime = 0;
  } state;

  CRGB prevColors[NUM_LEDS];

public:
  void startAnimation() override
  {
    state.stage = 0;
    state.isDone = false;
    state.currentLEDPos = DASH_BOTTOM_NUM_LEDS / 2;
    state.stageStartTime = millis();

    fill_solid(LedRegistry::dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, CRGB::Black);
    fill_solid(LedRegistry::door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, CRGB::Black);
    fill_solid(LedRegistry::door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, CRGB::Black);
    fill_solid(LedRegistry::door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, CRGB::Black);
    fill_solid(LedRegistry::door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, CRGB::Black);
    fill_solid(LedRegistry::charging_station_leds, NUM_LEDS, CRGB::Black);
    lastUpdateTime = millis();
  }

  void tickAnimation() override
  {
    unsigned long now = millis();
    ITheme *theme = LedManager::CurrentTheme;
    bool isNight = LedManager::IS_NIGHT_MODE;

    switch (state.stage)
    {
    case 0: // Soft Wake
    {
      uint16_t elapsed = now - state.stageStartTime;
      float t = min(1.0f, (float)elapsed / STAGE1_DURATION);
      uint8_t brightness = cubicEaseInOut(t) * 60;
      CRGB baseColor = CRGB::White;
      baseColor.nscale8_video(brightness);

      fill_solid(LedRegistry::dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, baseColor);
      fill_solid(LedRegistry::door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, baseColor);
      fill_solid(LedRegistry::door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, baseColor);
      fill_solid(LedRegistry::door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, baseColor);
      fill_solid(LedRegistry::door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, baseColor);
      fill_solid(LedRegistry::charging_station_leds, NUM_LEDS, baseColor);

      if (elapsed >= STAGE1_DURATION)
      {
        state.stage++;
        state.stageStartTime = now;
        state.currentLEDPos = 0;
      }
      break;
    }
    case 1: // Symmetric Dash Reveal
    {
      if (now - lastUpdateTime >= 20)
      {
        int left = (DASH_BOTTOM_NUM_LEDS / 2) - state.currentLEDPos;
        int right = (DASH_BOTTOM_NUM_LEDS / 2) + state.currentLEDPos;

        CRGB color = theme->lowerDashboardColor;
        color.nscale8_video(isNight ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);

        if (left >= 0)
          LedRegistry::dash_bottom_leds[left] = color;
        if (right < DASH_BOTTOM_NUM_LEDS)
          LedRegistry::dash_bottom_leds[right] = color;

        state.currentLEDPos++;
        lastUpdateTime = now;

        if (left <= 0 && right >= DASH_BOTTOM_NUM_LEDS)
        {
          state.stage++;
          state.stageStartTime = now;
        }
      }
      break;
    }
    case 2: // Breathing Speakers and Pockets
    {
      uint16_t elapsed = now - state.stageStartTime;
      float wave = (sin8((elapsed % 2000) * 255 / 2000)) / 255.0f;
      uint8_t eased = cubicEaseInOut(wave) * 64;
      CRGB breathColor = theme->doorPocketColorV2;
      breathColor.nscale8_video(isNight ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS);
      breathColor.nscale8_video(eased);

      fill_solid(LedRegistry::door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, breathColor);
      fill_solid(LedRegistry::door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, breathColor);

      if (elapsed >= STAGE3_DURATION)
      {
        state.stage++;
        state.stageStartTime = now;
        state.currentLEDPos = 0;
      }
      break;
    }
    case 3: // Charging Station Flow Gradient
    {
      if (now - lastUpdateTime >= 30 && state.currentLEDPos < NUM_LEDS)
      {
        for (int i = 0; i <= state.currentLEDPos; i++)
        {
          uint8_t blendAmount = map(i, 0, NUM_LEDS, 0, 255);
          CRGB color = blend(CRGB::White, theme->chargingStationColor, blendAmount);
          color.nscale8_video(isNight ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS);
          LedRegistry::charging_station_leds[i] = color;
        }
        state.currentLEDPos++;
        lastUpdateTime = now;
      }

      if (state.currentLEDPos >= NUM_LEDS)
      {
        state.stage++;
        state.stageStartTime = now;
        for (int i = 0; i < NUM_LEDS; i++)
          prevColors[i] = LedRegistry::charging_station_leds[i];
      }
      break;
    }
    case 4: // Final Sync Fade-In
    {
      uint16_t elapsed = now - state.stageStartTime;
      float t = min(1.0f, (float)elapsed / STAGE5_BLEND_DURATION);

      for (int i = 0; i < DASH_BOTTOM_NUM_LEDS; i++)
      {
        CRGB target = theme->lowerDashboardColor;
        target.nscale8_video(isNight ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
        LedRegistry::dash_bottom_leds[i] = blend(LedRegistry::dash_bottom_leds[i], target, t * 255);
      }

      for (int i = 0; i < NUM_LEDS_DOOR_FL_POCKET; i++)
      {
        CRGB target = theme->doorPocketColorV2;
        target.nscale8_video(isNight ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS);
        LedRegistry::door_fl_leds_pocket[i] = blend(LedRegistry::door_fl_leds_pocket[i], target, t * 255);
      }

      for (int i = 0; i < NUM_LEDS_DOOR_FR_POCKET; i++)
      {
        CRGB target = theme->doorPocketColor;
        target.nscale8_video(isNight ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS);
        LedRegistry::door_fr_leds_pocket[i] = blend(LedRegistry::door_fr_leds_pocket[i], target, t * 255);
      }

      for (int i = 0; i < NUM_LEDS_DOOR_FL_SPEAKER; i++)
      {
        CRGB target = theme->doorSpeakerColor;
        target.nscale8_video(isNight ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS);
        LedRegistry::door_fl_leds_speaker[i] = blend(LedRegistry::door_fl_leds_speaker[i], target, t * 255);
      }

      for (int i = 0; i < NUM_LEDS_DOOR_FR_SPEAKER; i++)
      {
        CRGB target = theme->doorSpeakerColor;
        target.nscale8_video(isNight ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS);
        LedRegistry::door_fr_leds_speaker[i] = blend(LedRegistry::door_fr_leds_speaker[i], target, t * 255);
      }

      for (int i = 0; i < NUM_LEDS; i++)
      {
        CRGB target = theme->chargingStationColor;
        target.nscale8_video(isNight ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS);
        LedRegistry::charging_station_leds[i] = blend(prevColors[i], target, t * 255);
      }

      if (elapsed >= STAGE5_BLEND_DURATION)
      {
        state.isDone = true;
        isCompleted = true;

        Log::println("StartupAnimation completed");

        delete LedManager::currentAnimation;
        LedManager::currentAnimation = new DrivingLoopAnimation();
        LedManager::currentAnimation->startAnimation();
      }
      break;
    }
    }
  }
};
