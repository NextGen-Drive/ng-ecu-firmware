#pragma once

struct NeoWhiteTheme : ITheme
{
public:
    NeoWhiteTheme()
    {
        chargingStationColor = CRGB(180, 255, 255); // Soft Türkis
        doorPocketColor = CRGB(200, 240, 255);      // Eisblau
        doorPocketColorV2 = CRGB(160, 210, 255);    // tieferes Blau
        doorSpeakerColor = CRGB(140, 190, 255);     // leicht kühl
        lowerDashboardColor = CRGB(180, 220, 255);  // Hauptfarbe
    }
};