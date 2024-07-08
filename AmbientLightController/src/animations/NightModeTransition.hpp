class NightModeTransition : public Animation {
  private:
    unsigned int transitionDuration = 1500;

  public:
   void tickAnimation() override {
     if (this->isCompleted) return;

     unsigned long elapsedTime = millis() - animationStartTime;

     transitionStrip(elapsedTime, charging_station_leds, NUM_LEDS, MOBILE_CHRGR_LED_DAY_BRIGHTNESS, MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS, CurrentTheme->chargingStationColor);
     transitionStrip(elapsedTime, door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, DOOR_SPEAKER_DAY_BRIGHTNESS, DOOR_SPEAKER_NIGHT_BRIGHTNESS, CurrentTheme->doorSpeakerColor);
     transitionStrip(elapsedTime, door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, DOOR_POCKET_DAY_BRIGHTNESS, DOOR_POCKET_NIGHT_BRIGHTNESS, CurrentTheme->doorPocketColor);
     transitionStrip(elapsedTime, dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, DASH_BOTTOM_LED_DAY_BRIGHTNESS, DASH_BOTTOM_LED_NIGHT_BRIGHTNESS, CurrentTheme->lowerDashboardColor);

     transitionStrip(elapsedTime, door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, DOOR_SPEAKER_DAY_BRIGHTNESS, DOOR_SPEAKER_NIGHT_BRIGHTNESS, CurrentTheme->doorSpeakerColor);
     transitionStrip(elapsedTime, door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, DOOR_POCKET_DAY_BRIGHTNESS, DOOR_POCKET_NIGHT_BRIGHTNESS, CurrentTheme->doorPocketColor);

     if (millis() >= animationStartTime + transitionDuration) {
       this->isCompleted = true;
     }
   }

   void transitionStrip(unsigned long elapsedTime, CRGB strip[], uint8_t num_leds, uint8_t day_brightness, uint8_t night_brightness, CRGB color) {
     uint8_t targetBrightness = IS_NIGHT_MODE ? night_brightness : day_brightness;
     uint8_t initialBrightness = !IS_NIGHT_MODE ? night_brightness : day_brightness;

     AnimationManager::fadeTo(elapsedTime, transitionDuration, strip, num_leds, initialBrightness, targetBrightness, color);
   }
};