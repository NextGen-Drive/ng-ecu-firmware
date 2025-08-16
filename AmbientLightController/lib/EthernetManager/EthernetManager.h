#pragma once
#include <Arduino.h>
#include <ETH.h>
#include "AsyncUDP.h"
#include "LogManager.hpp"

class EthernetManager
{
public:
    static void init(uint8_t id);

private:
    static String LogPrefix;
};
