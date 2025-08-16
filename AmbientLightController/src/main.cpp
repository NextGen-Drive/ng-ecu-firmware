#include <FastLED.h>

#include "LogManager.hpp"

#include "EthernetManager.h"

#ifdef CLIENT
#include "ClientNetworkManager.h"
#endif

#ifdef SERVER
#include "ServerManager.h"
#endif

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
  delay(1000);
  Log::println("Starting setup");

#ifdef CLIENT
  EthernetManager::init(2);
  ClientNetworkManager::init(2);
#endif

#ifdef SERVER
  EthernetManager::init(1);
  ServerManager::init();
#endif

#ifdef LED
  LedRegistry::init();
  LedManager::init();

  LedManager::currentAnimation = new StartupAnimation();
  LedManager::currentAnimation->startAnimation();
#endif

  Log::println("Finished setup");
}

void loop()
{
#ifdef CLIENT
  ClientNetworkManager::tick();
#endif

#ifdef SERVER
  ServerManager::tick();
#endif

#ifdef LED
  LedManager::tick();
#endif
}