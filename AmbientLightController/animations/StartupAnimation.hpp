class StartupAnimation : public Animation {
  private:
    unsigned int stage = 0;
    int currentLEDPos = 0;
    unsigned long endBrightnessStageStartTime = 0;
    const unsigned int STAGE2_DURATION = 1000; // in ms

  public:
    void tickAnimation() override {
      unsigned long currentTime = millis();

      // Dot animation from left to right
      if (stage == 0 && currentTime - lastUpdateTime >= 7) {
        if (currentLEDPos != DASH_BOTTOM_NUM_LEDS) {
          dash_bottom_leds[currentLEDPos + 1] = CRGB::Black;
        }
        dash_bottom_leds[currentLEDPos] = adjustBrightness(CRGB::White, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
        currentLEDPos--;
        lastUpdateTime = millis();

        if (currentLEDPos == -1) {
          stage++;
        }
      // Fade in from right to left
      } else if (stage == 1 && currentTime - lastUpdateTime >= 7) {
        dash_bottom_leds[currentLEDPos] = adjustBrightness(CurrentTheme->lowerDashboardColor, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
        currentLEDPos++;
        lastUpdateTime = millis();

        if (currentLEDPos == DASH_BOTTOM_NUM_LEDS + 1) {
          stage++;
          endBrightnessStageStartTime = millis();
          currentLEDPos = DASH_BOTTOM_NUM_LEDS;

          // Make sure everything was done correctly before as there's no animation for the dash in the next stage
          CRGB color = adjustBrightness(CurrentTheme->lowerDashboardColor, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
          fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, color); 
        }
      // fade in the charge strip and others
      } else if (stage == 2) {
        unsigned long elapsedStageTime = millis() - endBrightnessStageStartTime;

        // Mobile Charging Station
        AnimationManager::fadeIn(elapsedStageTime, STAGE2_DURATION, charging_station_leds, NUM_LEDS, MOBILE_CHRGR_LED_BRIGHTNESS, CurrentTheme->chargingStationColor);

        // Circular Door Speaker
        SharedAnimationParams dsParams = SharedAnimationParams(elapsedStageTime, STAGE2_DURATION, NUM_LEDS_DOOR_FL_SPEAKER, IS_NIGHT_MODE ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS, CurrentTheme->doorSpeakerColor);
        // Front Left
        AnimationManager::fadeInSeq(door_fl_leds_speaker, dsParams);
        // Front Right
        AnimationManager::fadeInSeq(door_fr_leds_speaker, dsParams);

        // Door Pocket
        // Front Left -> V2
        SharedAnimationParams dpv2Params = SharedAnimationParams(elapsedStageTime, STAGE2_DURATION, NUM_LEDS_DOOR_FL_POCKET, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS, CurrentTheme->doorPocketColorV2);
        AnimationManager::fadeInSeq(door_fl_leds_pocket, dpv2Params);
        // Front Right -> V1
        SharedAnimationParams dpv1Params = SharedAnimationParams(elapsedStageTime, STAGE2_DURATION, NUM_LEDS_DOOR_FR_POCKET, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS, CurrentTheme->doorPocketColor);
        AnimationManager::fadeInSeq(door_fr_leds_pocket, dpv1Params);

        lastUpdateTime = millis();

        if (elapsedStageTime >= STAGE2_DURATION) {
          stage++;
          isCompleted = true;
        }
      } else if (stage == 3) {
        fill_solid(charging_station_leds, NUM_LEDS, adjustBrightness(CurrentTheme->chargingStationColor, IS_NIGHT_MODE ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS)); 
        fill_solid(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, adjustBrightness(CurrentTheme->doorSpeakerColor, IS_NIGHT_MODE ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
        fill_solid(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, adjustBrightness(CurrentTheme->doorPocketColor, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
        fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, adjustBrightness(CurrentTheme->lowerDashboardColor, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS));

        fill_solid(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, adjustBrightness(CurrentTheme->doorSpeakerColor, IS_NIGHT_MODE ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
        fill_solid(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, adjustBrightness(CurrentTheme->doorPocketColorV2, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
      }
    }

    void startAnimation() override {
      currentLEDPos = DASH_BOTTOM_NUM_LEDS;
      fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, CRGB::Black);
      fill_solid(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, CRGB::Black);
      fill_solid(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, CRGB::Black);
      fill_solid(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, CRGB::Black);
      fill_solid(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, CRGB::Black);
      fill_solid(charging_station_leds, NUM_LEDS, CRGB::Black);
      lastUpdateTime = millis();
    }
};