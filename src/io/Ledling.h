#include <Arduino.h>


class Ledling_c
{
private:
    /* data */
    byte sat;
    long tTriggered;

public:
    // Ledling_c(/* args */);
    void trigger();
    void update();
    byte read();
};

// Ledling_c::Ledling_c(/* args */)
// {
// }

void Ledling_c::trigger()
{
    tTriggered = millis();
}

byte Ledling_c::read()
{
    return sat;
}

void Ledling_c::update()
{
    if (millis() < tTriggered + 200)
        sat = constrain(map(millis(), tTriggered, tTriggered + 200, 0, 260), 0, 200);
    else sat = 255;
}

Ledling_c usbLed;
Ledling_c inLed;
Ledling_c outLed;

void updateLedlings()
{
    usbLed.update();
    inLed.update();
    outLed.update();
}

void tickMidiOutIndicator()
{
    if(!digitalRead(PLUG_INDIC_PIN)) outLed.trigger();
    // Serial.println(digitalRead(PLUG_INDIC_PIN));
}
