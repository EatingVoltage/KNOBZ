#include <Arduino.h>

struct indicator_t
{
    long t0;
    int dur = 20;
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
        if (millis() - midiInIndicator.t0 > midiInIndicator.dur)
        {
            midiInIndicator.active = false;
        }
        setPixel(0, 0, 0, 255);
    }
    if(midiOutIndicator.active) // timeout
    {
        if (millis() - midiOutIndicator.t0 > midiOutIndicator.dur)
        {
            midiOutIndicator.active = false;
        }
        setPixel(1, 0, 0, 255);
    }
    if(USBIndicator.active) // timeout
    {
        if (millis() - USBIndicator.t0 > USBIndicator.dur)
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