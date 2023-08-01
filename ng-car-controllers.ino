#include <FastLED.h>

#define NUM_LEDS 30
#define DATA_PIN 8
#define DATA_PIN_CHARGING_STATION_LIGHT 12
#define NUM_LED_MIDDLE 16

#define DATA_PIN_DOOR_FR 4
#define NUM_LEDS_DOOR_FR 7

#define IS_DASH_LED_ENABLED false

#define MOBILE_CHRGR_LED_DAY_BRIGHTNESS 255
#define MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS 255

#define DATA_PIN_DOOR_FR_SPEAKER 4
#define NUM_LEDS_DOOR_FR_SPEAKER 30

#define DATA_PIN_DOOR_FR_POCKET 7
#define NUM_LEDS_DOOR_FR_POCKET 7

// Communication
#define RXp2 16
#define TXp2 17

CRGB leds[NUM_LEDS];
CRGB charging_station_leds[NUM_LEDS];
CRGB door_fr_leds[NUM_LEDS_DOOR_FR];
CRGB door_fr_leds_speaker[NUM_LEDS_DOOR_FR_SPEAKER];
CRGB door_fr_leds_pocket[NUM_LEDS_DOOR_FR_POCKET];

CRGB adjustBrightness(CRGB color, uint8_t brightness) {
  // Scale the brightness of the color using the nscale8_video() function
  color.nscale8_video(brightness);
  return color;
}

class LightStripBase {
  public:
    /*
    * Starts initializing the led strip
    */
    virtual void Initialize() = 0;
};

template<template<uint8_t Data_Pin, EOrder RGB_ORDER>class LED_Type, uint8_t Num_Leds, uint8_t Data_Pin>
class LightStrip : public LightStripBase {
  private:
    CRGB leds[Num_Leds];

  public:
    LightStrip(const char* name) {

    }

    /*
    * Starts initializing the led strip
    */
    void Initialize() {
      FastLED.addLeds<LED_Type, Data_Pin>(leds, Num_Leds); 
    }
};

LightStripBase* lights[] = {
  &LightStrip<WS2811, NUM_LEDS, DATA_PIN>("dash_ring"),
  &LightStrip<WS2811, NUM_LEDS, DATA_PIN_CHARGING_STATION_LIGHT>("charging_station"),
  &LightStrip<WS2811, NUM_LEDS_DOOR_FR_SPEAKER, DATA_PIN_DOOR_FR_SPEAKER>("door_fr_speaker"),
  &LightStrip<WS2811, NUM_LEDS_DOOR_FR_POCKET, DATA_PIN_DOOR_FR_POCKET>("door_fr_pocket"),
};

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

  public:
    void tickAnimation() override {
      unsigned long currentTime = millis();

      // Dot animation from left to right
      if (stage == 0 && currentTime - lastUpdateTime >= 25) {
        if (currentLEDPos != NUM_LEDS) {
          leds[currentLEDPos + 1] = CRGB::Black;
        }
        leds[currentLEDPos] = CRGB::White;
        currentLEDPos--;
        lastUpdateTime = millis();

        if (currentLEDPos == -1) {
          stage++;
        }
      // Fade in from right to left
      } else if (stage == 1 && currentTime - lastUpdateTime >= 25) {
        leds[currentLEDPos] = CRGB::White;
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

        fill_solid(leds, NUM_LEDS, color); 

        int chg_brightness = map(elapsedStageTime, 0, endBrightnessStageDuration, 0, MOBILE_CHRGR_LED_DAY_BRIGHTNESS);

        CRGB chg_color = adjustBrightness(CRGB::LightGreen, chg_brightness);

        fill_solid(charging_station_leds, NUM_LEDS, chg_color); 

        lastUpdateTime = millis();

        if (brightness <= 0) {
          stage++;
        }
      } else if (stage == 3) {
        CRGB color = CRGB::Red;

  if (IS_DASH_LED_ENABLED) {
    fill_solid(leds, NUM_LEDS, adjustBrightness(color, 128)); 
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black); 
  }

  fill_solid(charging_station_leds, NUM_LEDS, adjustBrightness(CRGB::LightBlue, MOBILE_CHRGR_LED_DAY_BRIGHTNESS)); 
      }
    }

    void startAnimation() override {
      currentLEDPos = NUM_LEDS;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
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

          leds[currentLEDPosLeft] = bgColor;
          leds[currentLEDPosRight] = bgColor;
        } else {
          leds[sparkPosLeft] = bgColor;
        leds[sparkPosRight] = bgColor;

        sparkPosLeft = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, NUM_LEDS);
        sparkPosRight = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, 0);

        if (sparkPosLeft > NUM_LEDS || sparkPosRight < 0) {
          sparkStartTime = millis();
          elapsedSparkTime = millis() - sparkStartTime;
          sparkPosLeft = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, NUM_LEDS);
          sparkPosRight = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, 0);
        }

        leds[sparkPosLeft] = CRGB::Yellow;
        leds[sparkPosRight] = CRGB::Yellow;
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

      fill_solid(leds, NUM_LEDS, CRGB::Black);
      lastUpdateTime = millis();
      fadeInStartTime = millis();
      sparkStartTime = millis();
    }
};

static Animation* currentAnimation;
static unsigned long previousTime = 0;

void setup() { 
  Serial.begin(115200);
  //Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  
  Serial.println("Setting up...");
  FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS); 
  FastLED.addLeds<WS2811, DATA_PIN_CHARGING_STATION_LIGHT>(charging_station_leds, NUM_LEDS); 
  //FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FR>(door_fr_leds, NUM_LEDS_DOOR_FR); 
  FastLED.addLeds<WS2811, DATA_PIN_DOOR_FR_SPEAKER>(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER);
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FR_POCKET>(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET);

  FastLED.clear();
  FastLED.setBrightness(255);

  currentAnimation = new StartupAnimation();
  currentAnimation->startAnimation();
}

void loop() { 
  currentAnimation->tickAnimation();

  fill_solid(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, adjustBrightness(CRGB::Blue, 255));
  fill_solid(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, adjustBrightness(CRGB::White, 255));

  FastLED.show();

  return;
}

// Helps identifying the number of leds
void identify_leds() {
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

void plaid_animation_start() {
  // ToDo: dim brightness of previous state down
  FastLED.clear();

  // leds are relative to the middle led
  int led_right = NUM_LED_MIDDLE - 1;
  int led_left = NUM_LED_MIDDLE + 1;

  // Set middle led state
  leds[NUM_LED_MIDDLE] = CRGB::DarkRed; 
  FastLED.show(); 
  delay(300);

  // ToDo: ramp up exponentially instead
  while (led_left != NUM_LEDS + 1) {
    leds[led_right] = CRGB::DarkRed;
    leds[led_left] = CRGB::DarkRed;
    FastLED.show();

    delay(300);

    led_right--;
    led_left++;
  }
}