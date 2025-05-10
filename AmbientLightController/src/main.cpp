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
#include <stdlib.h>

extern char _end;
extern "C" char* sbrk(int incr);

int freeMemory() {
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
}

void setup()
{
  Log::init();
  Log::println("Starting setup");

  pinMode(LED_BUILTIN, OUTPUT);

  LedRegistry::init();
  LedManager::init();
  I2CManager::init();

  LedManager::currentAnimation = new StartupAnimation();
  LedManager::currentAnimation->startAnimation();

  Log::println("Finished setup");
}

static unsigned long lastBlink = 0;
static bool ledState = false;

void loop()
{
  Log::println("loop");

  LedManager::tick();
  I2CManager::tick();
}