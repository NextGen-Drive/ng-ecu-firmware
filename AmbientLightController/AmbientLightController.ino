#include <FastLED.h>
#include <Wire.h>

#include "themes/ITheme.hpp"
#include "themes/ChillTheme.hpp"
#include "themes/SportTheme.hpp"

#include "helper.hpp"

bool IS_NIGHT_MODE = true;
static ITheme* CurrentTheme = new ChillTheme();

#include "configs/LedConfig.hpp"
const int MOBILE_CHRGR_LED_BRIGHTNESS = IS_NIGHT_MODE ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS;

#include "helper/AnimationHelper.hpp"

#include "animations/Animation.hpp"
#include "animations/StartupAnimation.hpp"
#include "animations/NmModeTransitionAnimation.hpp"

#define DATA_PIN 8
#define DATA_PIN_CHARGING_STATION_LIGHT 12
#define NUM_LED_MIDDLE 16

#define DATA_PIN_DOOR_FR 4

#define IS_DASH_LED_ENABLED false

const int DOOR_POCKET_BRIGHTNESS = IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS;

// Communication
#define RXp2 16
#define TXp2 17

#define EXPECTED_COMM_BYTES 1

static Animation* currentAnimation;
static unsigned long previousTime = 0;

void setup() { 
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C communication
  
  Serial.println("Setting up...");
  FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS); 
  FastLED.addLeds<WS2811, DATA_PIN_CHARGING_STATION_LIGHT>(charging_station_leds, NUM_LEDS); 
  FastLED.addLeds<WS2811, DATA_PIN_DOOR_FR_SPEAKER>(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER);
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FR_POCKET, GRB>(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET);
  FastLED.addLeds<WS2812B, DASH_BOTTOM_PIN, GRB>(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS);
  FastLED.addLeds<WS2811, DATA_PIN_DOOR_FL_SPEAKER>(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER);
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FL_POCKET, GRB>(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET);

  FastLED.clear();
  FastLED.setBrightness(255);

  currentAnimation = new StartupAnimation();
  currentAnimation->startAnimation();
}

bool getBit(uint8_t targetByte, int position) {
  if (position >= 0 && position <= 7) {
    return (targetByte >> position) & 0x01; // Get the bit at the specified position
  } else {
    Serial.println("Invalid bit position.");
    return false; // Return a default value
  }
}

void loop() { 
  currentAnimation->tickAnimation();

  FastLED.show();

  Wire.requestFrom(8, EXPECTED_COMM_BYTES); 

  if (Wire.available()) {
    uint8_t buffer;
    Wire.readBytes(&buffer, EXPECTED_COMM_BYTES);

    bool isNightMode = getBit(buffer, 0);

    if (isNightMode) {
      if (!currentAnimation->isCompleted || IS_NIGHT_MODE) return;
      Serial.println("Starting night mode animation");
      delete currentAnimation;
      IS_NIGHT_MODE = true;
      currentAnimation = new NmModeTransitionAnimation();
      currentAnimation->startAnimation();
    } else {
      if (!currentAnimation->isCompleted || !IS_NIGHT_MODE) return;
      Serial.println("Starting day mode animation");
      IS_NIGHT_MODE = false;
      delete currentAnimation;
      currentAnimation = new NmModeTransitionAnimation();
      currentAnimation->startAnimation();
    }
  }

  /*while (Wire.available()) {
    String c = Wire.readString();
    Serial.println((char)Wire.read());

    if (c == "NM_A") {
      if (!currentAnimation->isCompleted) return;
      IS_NIGHT_MODE = true;
      currentAnimation = new NmModeTransitionAnimation();
      currentAnimation->startAnimation();
    } else if (c == "NM_D") {
      if (!currentAnimation->isCompleted) return;
      IS_NIGHT_MODE = false;
      currentAnimation = new NmModeTransitionAnimation();
      currentAnimation->startAnimation();
    }
  }*/

  return;
}