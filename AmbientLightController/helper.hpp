CRGB adjustBrightness(CRGB color, uint8_t brightness) {
  // Scale the brightness of the color using the nscale8_video() function
  color.nscale8_video(brightness);
  return color;
}