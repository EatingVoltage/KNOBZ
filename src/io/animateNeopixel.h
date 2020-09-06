
#include <Arduino.h>

void animateNeopixel()
{
    int hue = 0;

    for (byte i = 0; i < 36; i++)
    {
        hue += ((knobs[i].val / 6) % 255);
        hue = hue % 255;
    }

    hue = (hue+50)%255;

    setPixel(0, hue, 255, 255);
    setPixel(3, (hue + 10)%255, 255, 255);
    setPixel(2, (hue +30)%255, 255, 255);
    setPixel(1, (hue + 40)%255, 255, 255);
}