#include <Arduino.h>


class Ledling_c
{
private:
    /* data */
    byte sat;
    long tTriggered;

public:
    // Ledling_c(/* args */);
    void trigger(long tNow);
    void update(long tNow);
    byte read();
};

// Ledling_c::Ledling_c(/* args */)
// {
// }

void Ledling_c::trigger(long tNow)
{
    tTriggered = tNow;
}

byte Ledling_c::read()
{
    return sat;
}

void Ledling_c::update(long tNow)
{
    if (tNow < tTriggered + 200)
        sat = constrain(map(tNow, tTriggered, tTriggered + 200, 0, 260), 0, 200);
    else sat = 255;
}

Ledling_c usbLed;
Ledling_c inLed;
Ledling_c outLed;

void updateLedlings()
{
    usbLed.update(tNow);
    inLed.update(tNow);
    outLed.update(tNow);
}

void tickMidiOutIndicator(long tNow)
{
    if(!digitalRead(PLUG_INDIC_PIN)) outLed.trigger(tNow);
    // Serial.println(digitalRead(PLUG_INDIC_PIN));
}
