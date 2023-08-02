#include <FastLED.h>
#include "LightStrip.h"
#include "Animations/StartupAnimation.h"

// Communication
#define RXp2 16
#define TXp2 17

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

  FastLED.show();

  return;
}

// Helps identifying the number of leds
void identify_leds(CRGB leds[]) {
  uint8_t num_leds = 0;

  FastLED.clear();

  int i = 0;
  for (i = 0; i < num_leds; i++) {
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