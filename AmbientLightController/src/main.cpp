#include <FastLED.h>

#include "managers/LogManager.hpp"
#include "managers/I2CManager.hpp"
#include "managers/LedManager.hpp"
#include "managers/LedRegistry.hpp"

#include "animations/Animation.hpp"

#include "themes/ITheme.hpp"
#include "themes/ChillTheme.hpp"
#include "themes/SportTheme.hpp"

#include "helper.hpp"

#include "configs/LedConfig.hpp"

#include "helper/AnimationHelper.hpp"

#include "animations/StartupAnimation.hpp"

#define IS_DASH_LED_ENABLED false

int availableMemory() {
    // Use 1024 with ATmega168
    int size = 2048;
    byte *buf;
    while ((buf = (byte *) malloc(--size)) == NULL);
        free(buf);
    return size;
}

void setup() { 
  Log::init();
  Log::println(F("Starting setup"));

  LedRegistry::init();
  LedManager::init();
  I2CManager::init();

  //Log::println(String(availableMemory()));
  
  LedManager::currentAnimation = new StartupAnimation();
  LedManager::currentAnimation->startAnimation();

  Log::println(F("Finished setup"));
}

void loop() { 
  return;
  LedManager::tick();

  I2CManager::tick();
}