#pragma once

#include "helper.hpp"

struct SharedAnimationParams {
  public: 
    unsigned long elapsedTime; 
    unsigned long duration;
    uint8_t num_leds;
    uint8_t target_brightness;
    CRGB target_color;
    SharedAnimationParams(unsigned long elapsedTime, unsigned long duration, uint8_t num_leds, uint8_t target_brightness, CRGB target_color) 
    : elapsedTime(elapsedTime), duration(duration), num_leds(num_leds), target_brightness(target_brightness), target_color(target_color) {
    }
};

struct AnimationManager {
  public:
    // Fades the whole strip in one piece (from initial_brightness to target_brightness)
    static void fadeTo(unsigned long elapsedTime, unsigned long duration, CRGB strip[], uint8_t num_leds, uint8_t initial_brightness, uint8_t target_brightness, CRGB target_color) {
      int current_brightness = map(elapsedTime, 0, duration, initial_brightness, target_brightness);
      CRGB current_color = adjustBrightness(target_color, current_brightness);
      fill_solid(strip, num_leds, current_color); 
    }

    // Fades in the whole strip in one piece (from 0)
    static void fadeIn(unsigned long elapsedTime, unsigned long duration, CRGB strip[], uint8_t num_leds, uint8_t target_brightness, CRGB target_color) {
      int current_brightness = map(elapsedTime, 0, duration, 0, target_brightness);
      CRGB current_color = adjustBrightness(target_color, current_brightness);
      fill_solid(strip, num_leds, current_color); 
    }

    // Fades in the whole strip in one piece (from 0)
    static void fadeIn(CRGB strip[], SharedAnimationParams p) {
      fadeIn(p.elapsedTime, p.duration, strip, p.num_leds, p.target_brightness, p.target_color);
    }

    // Fades in the strip LED by LED
    static void fadeInSeq(unsigned long elapsedTime, unsigned long duration, CRGB strip[], uint8_t num_leds, uint8_t target_brightness, CRGB target_color) {
      int speaker_active_led = map(elapsedTime, 0, duration, 0, num_leds);
      strip[speaker_active_led] = adjustBrightness(target_color, target_brightness);
    }

    static void fadeInSeq(CRGB strip[], SharedAnimationParams p) {
      fadeInSeq(p.elapsedTime, p.duration, strip, p.num_leds, p.target_brightness, p.target_color);
    }
};