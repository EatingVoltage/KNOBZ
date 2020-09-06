#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#define LED_PIN 13
#define NUMPIXELS 4 // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setPixel(byte index, byte hue, byte sat, byte val) // hsv output
{
  int h = hue*256;
  pixels.setPixelColor(index, pixels.ColorHSV(h, sat, val));
}
