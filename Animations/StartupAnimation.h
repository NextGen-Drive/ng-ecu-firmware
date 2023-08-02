#include "Animation.h"
#include "../LightStripBase.h"
#include "../LightStripHelper.h"

class StartupAnimation : public Animation {
  private:
    unsigned int stage = 0;
    int currentLEDPos = 0;
    unsigned long endBrightnessStageStartTime = 0;
    unsigned int endBrightnessStageDuration = 1000; // in ms
    LightStripBase* dash_ring;
    LightStripBase* chargingStationLightStrip;
    LightStripBase* door_fr_leds_speaker;
    LightStripBase* door_fr_leds_pocket;

  public:
    void tickAnimation() override {
      unsigned long currentTime = millis();

      // Dot animation from left to right
      if (stage == 0 && currentTime - lastUpdateTime >= 25) {
        if (currentLEDPos != dash_ring->getNumLeds()) {
          dash_ring->leds[currentLEDPos + 1] = CRGB::Black;
        }
        dash_ring->leds[currentLEDPos] = CRGB::White;
        currentLEDPos--;
        lastUpdateTime = millis();

        if (currentLEDPos == -1) {
          stage++;
        }
      // Fade in from right to left
      } else if (stage == 1 && currentTime - lastUpdateTime >= 25) {
        dash_ring->leds[currentLEDPos] = CRGB::White;
        currentLEDPos++;
        lastUpdateTime = millis();

        if (currentLEDPos == dash_ring->getNumLeds() + 1) {
          stage++;
          endBrightnessStageStartTime = millis();
          currentLEDPos = dash_ring->getNumLeds();
        }
      // Fade out the whole dash strip and fade in the charge strip
      } else if (stage == 2) {
        unsigned long elapsedStageTime = millis() - endBrightnessStageStartTime;

        int brightness = map(elapsedStageTime, 0, endBrightnessStageDuration, 255, 0);

        CRGB color = adjustBrightness(CRGB::White, brightness);

        fill_solid(dash_ring->leds, dash_ring->getNumLeds(), color); 

        // Fade in charging station led
        int chg_brightness = map(elapsedStageTime, 0, endBrightnessStageDuration, 0, MOBILE_CHRGR_LED_DAY_BRIGHTNESS);

        CRGB chg_color = adjustBrightness(CRGB::LightBlue, chg_brightness);

        fill_solid(chargingStationLightStrip->leds, chargingStationLightStrip->getNumLeds(), chg_color); 

        // Fade in circular speaker led
        int speaker_active_led = map(elapsedStageTime, 0, endBrightnessStageDuration, 0, door_fr_leds_speaker->getNumLeds());
        
        door_fr_leds_speaker->leds[speaker_active_led] = CRGB::Blue;

        // Fade in door pocket led
        int pocket_active_led = map(elapsedStageTime, 0, endBrightnessStageDuration, 0, door_fr_leds_pocket->getNumLeds());

        door_fr_leds_pocket->leds[pocket_active_led] = CRGB::White;

        lastUpdateTime = millis();

        if (brightness <= 0) {
          stage++;
        }
      } else if (stage == 3) {
        CRGB color = CRGB::Red;
        
        if (IS_DASH_LED_ENABLED) {
          fill_solid(dash_ring->leds, dash_ring->getNumLeds(), adjustBrightness(color, 128)); 
        } else {
          fill_solid(dash_ring->leds, dash_ring->getNumLeds(), CRGB::Black); 
        }
        fill_solid(chargingStationLightStrip->leds, chargingStationLightStrip->getNumLeds(), adjustBrightness(CRGB::LightBlue, MOBILE_CHRGR_LED_DAY_BRIGHTNESS)); 
        fill_solid(door_fr_leds_speaker->leds, door_fr_leds_speaker->getNumLeds(), adjustBrightness(CRGB::Blue, 255));
        fill_solid(door_fr_leds_pocket->leds, door_fr_leds_pocket->getNumLeds(), adjustBrightness(CRGB::White, 255));
      }
    }

    void startAnimation() override {
      dash_ring = findLightStrip(LightStripId::DashRing);
      chargingStationLightStrip = findLightStrip(LightStripId::ChargingStation);
      door_fr_leds_pocket = findLightStrip(LightStripId::Door_Fr_Pocket);
      door_fr_leds_speaker = findLightStrip(LightStripId::Door_Fr_Speaker);

      currentLEDPos = dash_ring->getNumLeds();
      fill_solid(dash_ring->leds, dash_ring->getNumLeds(), CRGB::Black);
      lastUpdateTime = millis();
    }
};