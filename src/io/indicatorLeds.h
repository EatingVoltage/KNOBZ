#include <Arduino.h>

#define INDICATOR_DUR 15

class IndicatorLed_c
{
private:
    /* data */
    unsigned int t0;
    bool active = false;
    byte index = 0;

public:
    IndicatorLed_c(byte _index)
    {
        index = _index;
    }
    void update()
    {
        if (active)
        {
            if (millis() - t0 > INDICATOR_DUR)
            {
                active = false;
            }
            setPixel(index, 0, 0, 255);
        }
    }
    void tick()
    {
        active = true;
        t0 = millis();
        setPixel(index, 0, 0, 255);
    }
    // IndicatorLed_c(byte _index);
};

IndicatorLed_c midiInIndicator(0);
IndicatorLed_c USBIndicator(2);
IndicatorLed_c midiOutIndicator(1);
