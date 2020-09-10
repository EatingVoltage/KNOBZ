#include <Arduino.h>

#define INDICATOR_DUR 15

struct indicator_t
{
    unsigned long t0;
    bool active = false;
};

indicator_t midiInIndicator;
indicator_t USBIndicator;
indicator_t midiOutIndicator;

// indicator leds
void updateIndicators()
{
    if(midiInIndicator.active) // timeout
    {
        if (millis() - midiInIndicator.t0 > INDICATOR_DUR)
        {
            midiInIndicator.active = false;
        }
        setPixel(0, 0, 0, 255);
    }
    if(midiOutIndicator.active) // timeout
    {
        if (millis() - midiOutIndicator.t0 > INDICATOR_DUR)
        {
            midiOutIndicator.active = false;
        }
        setPixel(1, 0, 0, 255);
    }
    if(USBIndicator.active) // timeout
    {
        if (millis() - USBIndicator.t0 > INDICATOR_DUR)
        {
            USBIndicator.active = false;
        }
        setPixel(2, 0, 0, 255);
    }
}

void tickMidiInIndicator()
{
    midiInIndicator.active = true;
    midiInIndicator.t0 = millis();
    setPixel(0, 0, 0, 255);
}

void tickMidiOutIndicator()
{
    midiOutIndicator.active = true;
    midiOutIndicator.t0 = millis();
    setPixel(1, 0, 0, 255);
}

void tickUSBIndicator()
{
    USBIndicator.active = true;
    USBIndicator.t0 = millis();
    setPixel(2, 0, 0, 255);
}