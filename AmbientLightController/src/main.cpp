#include <FastLED.h>

#include "managers/LogManager.hpp"
#include "managers/I2CManager.hpp"
#include "managers/LedManager.hpp"
#include "managers/LedRegistry.hpp"

#include "animations/Animation.hpp"

#include "themes/ITheme.hpp"
#include "themes/ChillTheme.hpp"
#include "themes/SportTheme.hpp"
#include "themes/NeoWhiteTheme.hpp"

#include "helper.hpp"

#include "configs/LedConfig.hpp"

#include "helper/AnimationHelper.hpp"

#include "animations/StartupAnimation.hpp"
#include "animations/DrivingLoopAnimation.hpp"

#define IS_DASH_LED_ENABLED false

void setup()
{
  Log::init();
  Log::println("Starting setup");

  pinMode(LED_BUILTIN, OUTPUT);

  I2CManager::init();
  LedRegistry::init();
  LedManager::init();

  LedManager::currentAnimation = new StartupAnimation();
  LedManager::currentAnimation->startAnimation();

  Log::println("Finished setup");
}

unsigned long previousMillis = 0;
int blinkStage = 0;

void loop()
{
  unsigned long currentMillis = millis();

  switch (blinkStage)
  {
  case 0:
    if (currentMillis - previousMillis >= 0)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      previousMillis = currentMillis;
      blinkStage = 1;
    }
    break;
  case 1:
    if (currentMillis - previousMillis >= 100)
    {
      digitalWrite(LED_BUILTIN, LOW);
      previousMillis = currentMillis;
      blinkStage = 2;
    }
    break;
  case 2:
    if (currentMillis - previousMillis >= 100)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      previousMillis = currentMillis;
      blinkStage = 3;
    }
    break;
  case 3:
    if (currentMillis - previousMillis >= 100)
    {
      digitalWrite(LED_BUILTIN, LOW);
      previousMillis = currentMillis;
      blinkStage = 4;
    }
    break;
  case 4:
    if (currentMillis - previousMillis >= 1000)
    {
      previousMillis = currentMillis;
      blinkStage = 0;
    }
    break;
  }

  LedManager::tick();
  I2CManager::tick();
}