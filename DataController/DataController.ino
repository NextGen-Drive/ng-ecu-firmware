#include <Wire.h>
#include "BluetoothSerial.h"
#include "ELMduino.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define NG_D_SERVICE_ID "a317a887-d070-48dc-a917-6f812a434fe1"
#define CURRENT_COLOR_CHARACTERISTIC_ID "fe430623-58ee-4ea9-8b1e-b6e57904048c"
#define ALLOW_CONNECT_OBD2_CHARACTERISTIC_ID "559AB6B6-A0D0-414A-9F0B-68D58E667749"

BluetoothSerial SerialBT;
#define ELM_PORT   SerialBT
#define DEBUG_PORT Serial

ELM327 myELM327;

obd_cmd_states obd_query_state = SEND_COMMAND;

unsigned long timeSInceLast;
bool flag = false;

class BleServerCallbacksHandler: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      pServer->startAdvertising(); // restart advertising
    };

    void onDisconnect(BLEServer* pServer) {
      pServer->startAdvertising(); // restart advertising
    }
};

void setup() {
  Wire.begin(8);
  Wire.onRequest(onRequest);
  DEBUG_PORT.begin(115200);
  ELM_PORT.begin("NG_CAN_MASTER", true);

  if (!ELM_PORT.connect("VEEPEAK"))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
  }

  if (!myELM327.begin(ELM_PORT, false, 2000, ISO_15765_11_BIT_500_KBAUD))
  {
    Serial.println("Couldn't connect to OBD scanner - Phase 2");
    while (1);
  }

  Serial.println("Connected to ELM327");

  // Fix: Not getting data for some commands which might take a bit longer (10 seconds might be abit excessive though lol)
  myELM327.timeout_ms = 10000;

  // Connect to IGMP
  int8_t setHeaderResult = myELM327.sendCommand_Blocking("AT SH 770");

  if (setHeaderResult != ELM_SUCCESS) {
    Serial.println("Something went wrong while trying to set the header");
  }

  timeSInceLast = millis();

  BLEDevice::init("NextGenDrive 001 Main ECU Dev");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BleServerCallbacksHandler());
  BLEService *pService = pServer->createService(NG_D_SERVICE_ID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CURRENT_COLOR_CHARACTERISTIC_ID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("122;133;144;1");

  BLECharacteristic *allowConnectObd2Characteristic = pService->createCharacteristic(
                                         ALLOW_CONNECT_OBD2_CHARACTERISTIC_ID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  allowConnectObd2Characteristic->setValue(true);                    

  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(NG_D_SERVICE_ID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Started Bluetooth server");
}

bool isWaiting = false;
unsigned long timeSinceLastCheck = 0;

char lightsFlag;

void loop() {
  /*if (timeSInceLast + 10000 < millis()) {
    timeSInceLast = millis();
    lightsFlag = flag ? '4' : '0';
    flag = !flag;
  }*/

  if (!isWaiting) {
    // Wait 1s for next request
    if (millis() - timeSinceLastCheck >= 1000) {
      myELM327.sendCommand("22 BC 03");
      isWaiting = true;
    }
    return;
  }

  myELM327.get_response();

		if (myELM327.nb_rx_state == ELM_SUCCESS)
		{
      timeSinceLastCheck = millis();
			//nb_query_state = SEND_COMMAND; // Reset the query state machine for next command
			
			myELM327.findResponse();

      int numSubstrings;

      char** result = splitString(myELM327.payload, numSubstrings);

      lightsFlag = String(result[2]).charAt(7);

      Serial.println(lightsFlag);

      /*for (int i = 0; i < numSubstrings; ++i) {
        printf("%s\n", result[i]);
    }*/

      isWaiting = false;
		}
		else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      myELM327.printError();
}

void onRequest() {
  if (lightsFlag != '0') {
    Serial.println("Should activate night mode");
    Wire.write("NM_A");
  } else {
    Serial.println("Should deactivate night mode");
    Wire.write("NM_D");
  }
}

char** splitString(const char* inputString, int& numSubstrings) {
    // Count the number of colons in the input string
    numSubstrings = 1;  // Start with 1 substring, as we're splitting at least once
    for (const char* ptr = inputString; *ptr; ++ptr) {
        if (*ptr == ':') {
            ++numSubstrings;
        }
    }

    // Allocate memory for the array of pointers
    char** substrings = new char*[numSubstrings];

    // Iterate through the input string and split at colons
    const char* start = inputString;
    int substringIndex = 0;
    for (const char* ptr = inputString; *ptr; ++ptr) {
        if (*ptr == ':') {
            // Calculate the length of the current substring
            int substringLength = ptr - start + 1;

            // Allocate memory for the substring
            substrings[substringIndex] = new char[substringLength];

            // Copy the substring to the newly allocated memory
            strncpy(substrings[substringIndex], start, substringLength - 1);
            substrings[substringIndex][substringLength - 1] = '\0';

            // Move the start pointer to the next character
            start = ptr + 1;

            ++substringIndex;
        }
    }

    // Handle the last substring
    int lastSubstringLength = strlen(start) + 1;
    substrings[substringIndex] = new char[lastSubstringLength];
    strcpy(substrings[substringIndex], start);

    return substrings;
}
