#pragma once

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