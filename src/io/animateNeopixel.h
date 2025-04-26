
#include <Arduino.h>

byte getHueFromKnobs()
{
    int hue = 0;

    for (byte i = 0; i < KNOB_AMT; i++)
    {
        hue += ((knob[i].getVal() / 6) % 255); // TODO check range here
        // hue = hue % 255;
    }

    hue = (hue + 50) % 255;
    return byte(hue);
}

void animateNeopixel(byte val = 255)
{
    byte hue = getHueFromKnobs();

    setPixelHsv(0, hue, 255, val);    // central led
    setPixelHsv(3, (hue + 10) % 255, inLed.read(), val);   // in led
    setPixelHsv(2, (hue + 30) % 255, outLed.read(), val); // out led
    setPixelHsv(1, (hue + 40) % 255, usbLed.read(), val); // usb led
}