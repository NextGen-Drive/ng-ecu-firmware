#include <Wire.h>

#include "LogManager.hpp"

#include "events/NightModeChangedEvent.hpp"

#define EXPECTED_COMM_BYTES 1

const unsigned long REQ_TIMEOUT = 2000;
const int8_t MAX_TIMEOUTS = 3;

enum CommState {
    NONE,
    FREE,
    WAITING,
    TIMEOUT,
};

/*
Interface for communicating with the DataController
*/
struct I2CManager
{
public:
    static void init()
    {
        Wire.begin(); // Initialize I2C communication
    }

    static void tick()
    {
        if (state == CommState::NONE || state == CommState::FREE) {
            Wire.requestFrom(8, EXPECTED_COMM_BYTES);
            state = CommState::WAITING;
            lastReqTime = millis();
        }

        if (Wire.available())
        {
            lastReqTime = 0;
            state = CommState::FREE;
            timeouts = 0;

            uint8_t buffer;
            Wire.readBytes(&buffer, EXPECTED_COMM_BYTES);

            bool isNightMode = getBit(buffer, 0);

            NightModeChangedEvent::handle(isNightMode);
        }

        unsigned long timeSinceReq = millis() - lastReqTime;
        if (state == CommState::WAITING && timeSinceReq > REQ_TIMEOUT) {
            timeouts++;
            state = CommState::FREE;

            if (timeouts > MAX_TIMEOUTS) {
                state = CommState::TIMEOUT;

                Log::error("I2CManager: DataController unreachable. I2C disabled for this session.");
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
    }

private:
    static CommState state;
    static unsigned long lastReqTime;
    static uint8_t timeouts;

    static bool getBit(uint8_t targetByte, int position)
    {
        if (position >= 0 && position <= 7)
        {
            return (targetByte >> position) & 0x01; // Get the bit at the specified position
        }
        else
        {
            Log::error("Invalid bit position.");
            return false; // Return a default value
        }
    }
};

CommState I2CManager::state = CommState::NONE;
unsigned long I2CManager::lastReqTime = 0;
uint8_t I2CManager::timeouts = 0;