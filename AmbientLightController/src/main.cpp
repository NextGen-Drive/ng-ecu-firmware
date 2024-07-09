#include <FastLED.h>

#include "managers/LogManager.hpp"
#include "managers/I2CManager.hpp"
#include "managers/LedManager.hpp"

#include "animations/Animation.hpp"

#include "themes/ITheme.hpp"
#include "themes/ChillTheme.hpp"
#include "themes/SportTheme.hpp"

#include "helper.hpp"

#include "configs/LedConfig.hpp"

#include "helper/AnimationHelper.hpp"

#include "animations/StartupAnimation.hpp"

#define IS_DASH_LED_ENABLED false

void setup() { 
  Log::init();
  Log::println("Starting setup");

  LedManager::init();
  I2CManager::init();

  LedManager::currentAnimation = new StartupAnimation();
  LedManager::currentAnimation->startAnimation();

  Log::println("Finished setup");
}

void loop() { 
  LedManager::tick();

  I2CManager::tick();
}