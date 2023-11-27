#include <FastLED.h>
#include <Wire.h>

#define NUM_LEDS 30
#define DATA_PIN 8
#define DATA_PIN_CHARGING_STATION_LIGHT 12
#define NUM_LED_MIDDLE 16

#define DATA_PIN_DOOR_FR 4
#define NUM_LEDS_DOOR_FR 7

#define IS_DASH_LED_ENABLED false
bool IS_NIGHT_MODE = true;

#pragma region LED Configuration

#define DASH_BOTTOM_PIN 2
#define DASH_BOTTOM_NUM_LEDS 128

#define DASH_BOTTOM_LED_DAY_BRIGHTNESS 255
#define DASH_BOTTOM_LED_NIGHT_BRIGHTNESS 30
#define DASH_BOTTOM_LED_COLOR CRGB::Blue

#define MOBILE_CHRGR_LED_DAY_BRIGHTNESS 255
#define MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS 20
#define MOBILE_CHRGR_LED_COLOR CRGB::White

#define DOOR_POCKET_DAY_BRIGHTNESS 255
#define DOOR_POCKET_NIGHT_BRIGHTNESS 20
#define DOOR_POCKET_COLOR CRGB::White
#define DOOR_POCKET_V2_COLOR CRGB::White

#define DOOR_SPEAKER_DAY_BRIGHTNESS 255
#define DOOR_SPEAKER_NIGHT_BRIGHTNESS 255
#define DOOR_SPEAKER_COLOR CRGB::Blue

#define DATA_PIN_DOOR_FR_SPEAKER 4
#define NUM_LEDS_DOOR_FR_SPEAKER 10

#define DATA_PIN_DOOR_FR_POCKET 7
#define NUM_LEDS_DOOR_FR_POCKET 7

#define DATA_PIN_DOOR_FL_SPEAKER 5
#define NUM_LEDS_DOOR_FL_SPEAKER 10

#define DATA_PIN_DOOR_FL_POCKET 13
#define NUM_LEDS_DOOR_FL_POCKET 53

#pragma endregion

const int MOBILE_CHRGR_LED_BRIGHTNESS = IS_NIGHT_MODE ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS;
const int DOOR_POCKET_BRIGHTNESS = IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS;

// Communication
#define RXp2 16
#define TXp2 17

#define EXPECTED_COMM_BYTES 1

CRGB leds[NUM_LEDS];
CRGB charging_station_leds[NUM_LEDS];
CRGB door_fr_leds[NUM_LEDS_DOOR_FR];
CRGB door_fr_leds_speaker[NUM_LEDS_DOOR_FR_SPEAKER];
CRGB door_fr_leds_pocket[NUM_LEDS_DOOR_FR_POCKET];
CRGB dash_bottom_leds[DASH_BOTTOM_NUM_LEDS];
CRGB door_fl_leds_speaker[NUM_LEDS_DOOR_FL_SPEAKER];
CRGB door_fl_leds_pocket[NUM_LEDS_DOOR_FL_POCKET];

CRGB adjustBrightness(CRGB color, uint8_t brightness) {
  // Scale the brightness of the color using the nscale8_video() function
  color.nscale8_video(brightness);
  return color;
}

struct ITheme {
  public:
    CRGB chargingStationColor;
    CRGB doorPocketColor;
    CRGB doorPocketColorV2;
    CRGB doorSpeakerColor;
    CRGB lowerDashboardColor;
};

struct ChillTheme : ITheme {
  public:
    ChillTheme() {
      chargingStationColor = CRGB::White;
      doorPocketColor = CRGB::White;
      doorPocketColorV2 = CRGB::LightBlue;
      doorSpeakerColor = CRGB::Blue;
      lowerDashboardColor = CRGB::Blue;
    }
};

struct SportTheme : ITheme {
  public:
    SportTheme() {
      chargingStationColor = CRGB::Red;
      doorPocketColor = CRGB::Red;
      doorPocketColorV2 = CRGB::Red;
      doorSpeakerColor = CRGB::Red;
      lowerDashboardColor = CRGB::Red;
    }
};

static ITheme* CurrentTheme = new ChillTheme();

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

class Animation {
  protected:
    unsigned long animationStartTime = 0;
    unsigned long lastUpdateTime = 0;

  public:
    bool isCompleted = false;
    virtual void tickAnimation() {

    }

    virtual void startAnimation() {
      animationStartTime = millis();
    }
};

class NmModeTransitionAnimation : public Animation {
  private:
    unsigned int transitionDuration = 1500;

  public:
   void tickAnimation() override {
     if (this->isCompleted) return;

     unsigned long elapsedTime = millis() - animationStartTime;

     transitionStrip(elapsedTime, charging_station_leds, NUM_LEDS, MOBILE_CHRGR_LED_DAY_BRIGHTNESS, MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS, CurrentTheme->chargingStationColor);
     transitionStrip(elapsedTime, door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, DOOR_SPEAKER_DAY_BRIGHTNESS, DOOR_SPEAKER_NIGHT_BRIGHTNESS, CurrentTheme->doorSpeakerColor);
     transitionStrip(elapsedTime, door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, DOOR_POCKET_DAY_BRIGHTNESS, DOOR_POCKET_NIGHT_BRIGHTNESS, CurrentTheme->doorPocketColor);
     transitionStrip(elapsedTime, dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, DASH_BOTTOM_LED_DAY_BRIGHTNESS, DASH_BOTTOM_LED_NIGHT_BRIGHTNESS, CurrentTheme->lowerDashboardColor);

     transitionStrip(elapsedTime, door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, DOOR_SPEAKER_DAY_BRIGHTNESS, DOOR_SPEAKER_NIGHT_BRIGHTNESS, CurrentTheme->doorSpeakerColor);
     transitionStrip(elapsedTime, door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, DOOR_POCKET_DAY_BRIGHTNESS, DOOR_POCKET_NIGHT_BRIGHTNESS, CurrentTheme->doorPocketColor);

     if (millis() >= animationStartTime + transitionDuration) {
       this->isCompleted = true;
     }
   }

   void transitionStrip(unsigned long elapsedTime, CRGB strip[], uint8_t num_leds, uint8_t day_brightness, uint8_t night_brightness, CRGB color) {
     uint8_t targetBrightness = IS_NIGHT_MODE ? night_brightness : day_brightness;
     uint8_t initialBrightness = !IS_NIGHT_MODE ? night_brightness : day_brightness;

     AnimationManager::fadeTo(elapsedTime, transitionDuration, strip, num_leds, initialBrightness, targetBrightness, color);
   }
};

class StartupAnimation : public Animation {
  private:
    unsigned int stage = 0;
    int currentLEDPos = 0;
    unsigned long endBrightnessStageStartTime = 0;
    const unsigned int STAGE2_DURATION = 1000; // in ms

  public:
    void tickAnimation() override {
      unsigned long currentTime = millis();

      // Dot animation from left to right
      if (stage == 0 && currentTime - lastUpdateTime >= 7) {
        if (currentLEDPos != DASH_BOTTOM_NUM_LEDS) {
          dash_bottom_leds[currentLEDPos + 1] = CRGB::Black;
        }
        dash_bottom_leds[currentLEDPos] = adjustBrightness(CRGB::White, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
        currentLEDPos--;
        lastUpdateTime = millis();

        if (currentLEDPos == -1) {
          stage++;
        }
      // Fade in from right to left
      } else if (stage == 1 && currentTime - lastUpdateTime >= 7) {
        dash_bottom_leds[currentLEDPos] = adjustBrightness(CurrentTheme->lowerDashboardColor, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
        currentLEDPos++;
        lastUpdateTime = millis();

        if (currentLEDPos == DASH_BOTTOM_NUM_LEDS + 1) {
          stage++;
          endBrightnessStageStartTime = millis();
          currentLEDPos = DASH_BOTTOM_NUM_LEDS;

          // Make sure everything was done correctly before as there's no animation for the dash in the next stage
          CRGB color = adjustBrightness(CurrentTheme->lowerDashboardColor, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
          fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, color); 
        }
      // fade in the charge strip and others
      } else if (stage == 2) {
        unsigned long elapsedStageTime = millis() - endBrightnessStageStartTime;

        // Mobile Charging Station
        AnimationManager::fadeIn(elapsedStageTime, STAGE2_DURATION, charging_station_leds, NUM_LEDS, MOBILE_CHRGR_LED_BRIGHTNESS, CurrentTheme->chargingStationColor);

        // Circular Door Speaker
        SharedAnimationParams dsParams = SharedAnimationParams(elapsedStageTime, STAGE2_DURATION, NUM_LEDS_DOOR_FL_SPEAKER, IS_NIGHT_MODE ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS, CurrentTheme->doorSpeakerColor);
        // Front Left
        AnimationManager::fadeInSeq(door_fl_leds_speaker, dsParams);
        // Front Right
        AnimationManager::fadeInSeq(door_fr_leds_speaker, dsParams);

        // Door Pocket
        // Front Left -> V2
        SharedAnimationParams dpv2Params = SharedAnimationParams(elapsedStageTime, STAGE2_DURATION, NUM_LEDS_DOOR_FL_POCKET, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS, CurrentTheme->doorPocketColorV2);
        AnimationManager::fadeInSeq(door_fl_leds_pocket, dpv2Params);
        // Front Right -> V1
        SharedAnimationParams dpv1Params = SharedAnimationParams(elapsedStageTime, STAGE2_DURATION, NUM_LEDS_DOOR_FR_POCKET, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS, CurrentTheme->doorPocketColor);
        AnimationManager::fadeInSeq(door_fr_leds_pocket, dpv1Params);

        lastUpdateTime = millis();

        if (elapsedStageTime >= STAGE2_DURATION) {
          stage++;
          isCompleted = true;
        }
      } else if (stage == 3) {
        fill_solid(charging_station_leds, NUM_LEDS, adjustBrightness(CurrentTheme->chargingStationColor, IS_NIGHT_MODE ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS)); 
        fill_solid(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, adjustBrightness(CurrentTheme->doorSpeakerColor, IS_NIGHT_MODE ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
        fill_solid(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, adjustBrightness(CurrentTheme->doorPocketColor, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
        fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, adjustBrightness(CurrentTheme->lowerDashboardColor, IS_NIGHT_MODE ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS));

        fill_solid(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, adjustBrightness(CurrentTheme->doorSpeakerColor, IS_NIGHT_MODE ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
        fill_solid(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, adjustBrightness(CurrentTheme->doorPocketColorV2, IS_NIGHT_MODE ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
      }
    }

    void startAnimation() override {
      currentLEDPos = DASH_BOTTOM_NUM_LEDS;
      fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, CRGB::Black);
      fill_solid(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, CRGB::Black);
      fill_solid(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, CRGB::Black);
      fill_solid(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, CRGB::Black);
      fill_solid(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, CRGB::Black);
      fill_solid(charging_station_leds, NUM_LEDS, CRGB::Black);
      lastUpdateTime = millis();
    }
};

class PlaidAnimation : public Animation {
  private:
    unsigned int stage = 0;
    int currentLEDPosRight = 0;
    int currentLEDPosLeft = 0;
    int sparkPositionLeft = 0;
    int sparkPositionRight = 0;

    unsigned long fadeInStartTime = 0;
    unsigned int fadeInDuration = 500; // in ms

    unsigned long sparkStartTime = 0;
    unsigned int sparkDuration = 800; // in ms

    int sparkPosLeft = 0;
    int sparkPosRight = 0;
    CRGB bgColor = CRGB::Red;

  public:
    void tickAnimation() override {

      // Was soll der Quatsch???
      // Einfache Animation mit Fade In auf rot von mitte nach rechts und mite nach links
      unsigned long currentTime = millis();
      unsigned long elapsedSparkTime = millis() - sparkStartTime;

      // fade in left and right from middle led
      if (stage == 0) {
        unsigned long elapsedStageTime = millis() - fadeInStartTime;

        if (currentLEDPosLeft < NUM_LEDS && currentLEDPosRight > 0) {
          currentLEDPosLeft = map(elapsedStageTime, 0, fadeInDuration, NUM_LED_MIDDLE, NUM_LEDS);
          currentLEDPosRight = map(elapsedStageTime, 0, fadeInDuration, NUM_LED_MIDDLE, 0);

          leds[currentLEDPosLeft] = bgColor;
          leds[currentLEDPosRight] = bgColor;
        } else {
          leds[sparkPosLeft] = bgColor;
        leds[sparkPosRight] = bgColor;

        sparkPosLeft = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, NUM_LEDS);
        sparkPosRight = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, 0);

        if (sparkPosLeft > NUM_LEDS || sparkPosRight < 0) {
          sparkStartTime = millis();
          elapsedSparkTime = millis() - sparkStartTime;
          sparkPosLeft = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, NUM_LEDS);
          sparkPosRight = map(elapsedSparkTime, 0, sparkDuration, NUM_LED_MIDDLE, 0);
        }

        leds[sparkPosLeft] = CRGB::Yellow;
        leds[sparkPosRight] = CRGB::Yellow;
        }

        lastUpdateTime = millis();

        if (currentLEDPosLeft == NUM_LEDS + 1 || currentLEDPosRight == -1) {
          stage++;
        }
      }
    }

    void startAnimation() override {
      FastLED.clear();
      currentLEDPosLeft = NUM_LED_MIDDLE;
      currentLEDPosRight = NUM_LED_MIDDLE;

      fill_solid(leds, NUM_LEDS, CRGB::Black);
      lastUpdateTime = millis();
      fadeInStartTime = millis();
      sparkStartTime = millis();
    }
};

static Animation* currentAnimation;
static unsigned long previousTime = 0;

void setup() { 
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C communication
  
  Serial.println("Setting up...");
  FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS); 
  FastLED.addLeds<WS2811, DATA_PIN_CHARGING_STATION_LIGHT>(charging_station_leds, NUM_LEDS); 
  FastLED.addLeds<WS2811, DATA_PIN_DOOR_FR_SPEAKER>(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER);
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FR_POCKET, GRB>(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET);
  FastLED.addLeds<WS2812B, DASH_BOTTOM_PIN, GRB>(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS);
  FastLED.addLeds<WS2811, DATA_PIN_DOOR_FL_SPEAKER>(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER);
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_FL_POCKET, GRB>(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET);

  FastLED.clear();
  FastLED.setBrightness(255);

  currentAnimation = new StartupAnimation();
  currentAnimation->startAnimation();
}

bool getBit(uint8_t targetByte, int position) {
  if (position >= 0 && position <= 7) {
    return (targetByte >> position) & 0x01; // Get the bit at the specified position
  } else {
    Serial.println("Invalid bit position.");
    return false; // Return a default value
  }
}

void loop() { 
  currentAnimation->tickAnimation();

  FastLED.show();

  Wire.requestFrom(8, EXPECTED_COMM_BYTES); 

  if (Wire.available()) {
    uint8_t buffer;
    Wire.readBytes(&buffer, EXPECTED_COMM_BYTES);

    bool isNightMode = getBit(buffer, 0);

    if (isNightMode) {
      if (!currentAnimation->isCompleted || IS_NIGHT_MODE) return;
      Serial.println("Starting night mode animation");
      delete currentAnimation;
      IS_NIGHT_MODE = true;
      currentAnimation = new NmModeTransitionAnimation();
      currentAnimation->startAnimation();
    } else {
      if (!currentAnimation->isCompleted || !IS_NIGHT_MODE) return;
      Serial.println("Starting day mode animation");
      IS_NIGHT_MODE = false;
      delete currentAnimation;
      currentAnimation = new NmModeTransitionAnimation();
      currentAnimation->startAnimation();
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

  return;
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

void plaid_animation_start() {
  // ToDo: dim brightness of previous state down
  FastLED.clear();

  // leds are relative to the middle led
  int led_right = NUM_LED_MIDDLE - 1;
  int led_left = NUM_LED_MIDDLE + 1;

  // Set middle led state
  leds[NUM_LED_MIDDLE] = CRGB::DarkRed; 
  FastLED.show(); 
  delay(300);

  // ToDo: ramp up exponentially instead
  while (led_left != NUM_LEDS + 1) {
    leds[led_right] = CRGB::DarkRed;
    leds[led_left] = CRGB::DarkRed;
    FastLED.show();

    delay(300);

    led_right--;
    led_left++;
  }
}
