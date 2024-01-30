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