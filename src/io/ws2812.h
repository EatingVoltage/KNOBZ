#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#define LED_PIN 13
#define NUMPIXELS 4 // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void neopixelBegin()
{
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(LED_BRIGHTNESS);
}

void setPixelHsv(byte index, byte h, byte s, byte v)
{ // hsv to rgb converter
  byte red, green, blue;

  h = (h * 192) / 256;           // 0..191
  unsigned int i = h / 32;       // We want a value of 0 thru 5
  unsigned int f = (h % 32) * 8; // 'fractional' part of 'i' 0..248 in jumps

  unsigned int sInv = 255 - s; // 0 -> 0xff, 0xff -> 0
  unsigned int fInv = 255 - f; // 0 -> 0xff, 0xff -> 0
  byte pv = v * sInv / 256;    // pv will be in range 0 - 255
  byte qv = v * (256 - s * f / 256) / 256;
  byte tv = v * (256 - s * fInv / 256) / 256;

  switch (i)
  {
  case 0:
    red = v;
    green = tv;
    blue = pv;
    break;
  case 1:
    red = qv;
    green = v;
    blue = pv;
    break;
  case 2:
    red = pv;
    green = v;
    blue = tv;
    break;
  case 3:
    red = pv;
    green = qv;
    blue = v;
    break;
  case 4:
    red = tv;
    green = pv;
    blue = v;
    break;
  case 5:
    red = v;
    green = pv;
    blue = qv;
    break;
  }
  pixels.setPixelColor(index, red, green, blue);
}
