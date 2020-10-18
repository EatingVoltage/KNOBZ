
#include <Arduino.h>

byte getHueFromKnobs()
{
    int hue = 0;

    for (byte i = 0; i < KNOB_AMT; i++)
    {
        hue += ((knobs[i].val / 6) % 255);
        hue = hue % 255;
    }

    hue = (hue + 50) % 255;
    return byte(hue);
}

void animateNeopixel(byte val=255)
{
    byte hue = getHueFromKnobs();

    setPixel(0, hue, 255, val);
    setPixel(3, (hue + 10) % 255, 255, val);
    setPixel(2, (hue + 30) % 255, 255, val);
    setPixel(1, (hue + 40) % 255, 255, val);
}