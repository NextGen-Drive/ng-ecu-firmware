#include <FastLED.h>

#define NUM_LEDS 100
#define DATA_PIN 4

CRGB leds[NUM_LEDS];

CRGB adjustBrightness(CRGB color, uint8_t brightness) {
  // Scale the brightness of the color using the nscale8_video() function
  color.nscale8_video(brightness);
  return color;
}

void setup() { 
  Serial.begin(115200);
  
  Serial.println("Setting up...");
  FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS); 

  FastLED.clear();
  FastLED.setBrightness(100);
}

void loop() { 
  fill_solid(leds, NUM_LEDS, adjustBrightness(CRGB::red, 255));

  FastLED.show();
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