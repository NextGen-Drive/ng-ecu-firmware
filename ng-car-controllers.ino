#include <FastLED.h>
#include "LightStrip.h"

// Communication
#define RXp2 16
#define TXp2 17

CRGB adjustBrightness(CRGB color, uint8_t brightness) {
  // Scale the brightness of the color using the nscale8_video() function
  color.nscale8_video(brightness);
  return color;
}

LightStripBase* findLightStrip(LightStripId id) {
  size_t arrLength = sizeof(lights) / sizeof(lights[0]);
  LightStripBase* foundElement = nullptr;

  for (size_t i = 0; i < arrLength; i++) {
    if (lights[i]->Id == id) {
      foundElement = lights[i];
      break; // Element found, exit the loop
    }
  }

  return foundElement;
}

class Animation {
  protected:
    unsigned long animationStartTime = 0;
    unsigned long lastUpdateTime = 0;

  public:
    virtual void tickAnimation() {

    }

    virtual void startAnimation() {

    }
};

class StartupAnimation : public Animation {
  private:
    unsigned int stage = 0;
    int currentLEDPos = 0;
    unsigned long endBrightnessStageStartTime = 0;
    unsigned int endBrightnessStageDuration = 1000; // in ms
    LightStripBase* dash_ring;
    LightStripBase* chargingStationLightStrip;

  public:
    void tickAnimation() override {
      unsigned long currentTime = millis();

      // Dot animation from left to right
      if (stage == 0 && currentTime - lastUpdateTime >= 25) {
        if (currentLEDPos != NUM_LEDS) {
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

        if (currentLEDPos == NUM_LEDS + 1) {
          stage++;
          endBrightnessStageStartTime = millis();
          currentLEDPos = NUM_LEDS;
        }
      // Fade out the whole dash strip and fade in the charge strip
      } else if (stage == 2) {
        unsigned long elapsedStageTime = millis() - endBrightnessStageStartTime;

        int brightness = map(elapsedStageTime, 0, endBrightnessStageDuration, 255, 0);

        CRGB color = adjustBrightness(CRGB::White, brightness);

        fill_solid(dash_ring->leds, NUM_LEDS, color); 

        int chg_brightness = map(elapsedStageTime, 0, endBrightnessStageDuration, 0, MOBILE_CHRGR_LED_DAY_BRIGHTNESS);

        CRGB chg_color = adjustBrightness(CRGB::LightGreen, chg_brightness);

        fill_solid(chargingStationLightStrip->leds, NUM_LEDS, chg_color); 

        lastUpdateTime = millis();

        if (brightness <= 0) {
          stage++;
        }
      } else if (stage == 3) {
        CRGB color = CRGB::Red;

  if (IS_DASH_LED_ENABLED) {
    fill_solid(dash_ring->leds, NUM_LEDS, adjustBrightness(color, 128)); 
  } else {
    fill_solid(dash_ring->leds, NUM_LEDS, CRGB::Black); 
  }

  fill_solid(chargingStationLightStrip->leds, NUM_LEDS, adjustBrightness(CRGB::LightBlue, MOBILE_CHRGR_LED_DAY_BRIGHTNESS)); 
      }
    }

    void startAnimation() override {
      dash_ring = findLightStrip(LightStripId::DashRing);

      currentLEDPos = NUM_LEDS;
      fill_solid(dash_ring->leds, NUM_LEDS, CRGB::Black);
      lastUpdateTime = millis();
    }
};

class PlaidAnimation : public Animation {
  private:
    unsigned int stage = 0;
    int currentLEDPosRight = 0;
    int currentLEDPosLeft = 0;
    int sparkPositionLeft = 0;
    int sparkPositionRight = 0;

    unsigned long fadeInStartTime = 0;
    unsigned int fadeInDuration = 500; // in ms

    unsigned long sparkStartTime = 0;
    unsigned int sparkDuration = 800; // in ms

    int sparkPosLeft = 0;
    int sparkPosRight = 0;
    CRGB bgColor = CRGB::Red;

    LightStripBase* lightStrip;

  public:
    void tickAnimation() override {

      // Was soll der Quatsch???
      // Einfache Animation mit Fade In auf rot von mitte nach rechts und mite nach links
      unsigned long currentTime = millis();
      unsigned long elapsedSparkTime = millis() - sparkStartTime;

      // fade in left and right from middle led
      if (stage == 0) {
        unsigned long elapsedStageTime = millis() - fadeInStartTime;

        if (currentLEDPosLeft < NUM_LEDS && currentLEDPosRight > 0) {
          currentLEDPosLeft = map(elapsedStageTime, 0, fadeInDuration, NUM_LED_MIDDLE, NUM_LEDS);
          currentLEDPosRight = map(elapsedStageTime, 0, fadeInDuration, NUM_LED_MIDDLE, 0);

          lightStrip->leds[currentLEDPosLeft] = bgColor;
          lightStrip->leds[currentLEDPosRight] = bgColor;
        } else {
          lightStrip->leds[sparkPosLeft] = bgColor;
        lightStrip->leds[sparkPosRight] = bgColor;

        sparkPosLeft = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, NUM_LEDS);
        sparkPosRight = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, 0);

        if (sparkPosLeft > NUM_LEDS || sparkPosRight < 0) {
          sparkStartTime = millis();
          elapsedSparkTime = millis() - sparkStartTime;
          sparkPosLeft = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, NUM_LEDS);
          sparkPosRight = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, 0);
        }

        lightStrip->leds[sparkPosLeft] = CRGB::Yellow;
        lightStrip->leds[sparkPosRight] = CRGB::Yellow;
        }

        lastUpdateTime = millis();

        if (currentLEDPosLeft == NUM_LEDS + 1 || currentLEDPosRight == -1) {
          stage++;
        }
      }
    }

    void startAnimation() override {
      FastLED.clear();
      currentLEDPosLeft = NUM_LED_MIDDLE;
      currentLEDPosRight = NUM_LED_MIDDLE;

      fill_solid(lightStrip->leds, NUM_LEDS, CRGB::Black);
      lastUpdateTime = millis();
      fadeInStartTime = millis();
      sparkStartTime = millis();
    }
};

static Animation* currentAnimation;
static unsigned long previousTime = 0;

static LightStripBase* door_fr_leds_speaker;
static LightStripBase* door_fr_leds_pocket;

void setup() { 
  Serial.begin(115200);
  //Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  Serial.println("Setting up...");

  size_t arrLength = sizeof(lights) / sizeof(lights[0]);

  for (size_t i = 0; i < arrLength; i++) {
    LightStripBase* lightStrip = lights[i];
    lightStrip->Initialize();
  }

  FastLED.clear();
  FastLED.setBrightness(255);

  currentAnimation = new StartupAnimation();
  currentAnimation->startAnimation();

  door_fr_leds_speaker = findLightStrip(LightStripId::Door_Fr_Speaker);
  door_fr_leds_pocket = findLightStrip(LightStripId::Door_Fr_Pocket);

  Serial.println("Setup was successful.");
}

void loop() { 
  currentAnimation->tickAnimation();

  fill_solid(door_fr_leds_speaker->leds, NUM_LEDS_DOOR_FR_SPEAKER, adjustBrightness(CRGB::Blue, 255));
  fill_solid(door_fr_leds_pocket->leds, NUM_LEDS_DOOR_FR_POCKET, adjustBrightness(CRGB::White, 255));

  FastLED.show();

  return;
}

// Helps identifying the number of leds
void identify_leds(CRGB leds[]) {
  FastLED.clear();

  int i = 0;
  for (i = 0; i < NUM_LEDS; i++) {
    Serial.println("IDENTIFY LED: Current LED num: " + i);

    leds[i] = CRGB::White; 
    FastLED.show(); 
    delay(1000);
    leds[i] = CRGB::Black; 
    delay(1000);
    leds[i] = CRGB::White; 
    FastLED.show(); 
    delay(1000);
    leds[i] = CRGB::Black; 
    delay(1000);
    leds[i] = CRGB::White; 
    FastLED.show(); 
    delay(1000);
    leds[i] = CRGB::Black; 
    delay(1000);
  }
}