#include <Arduino.h>

void showHeader() // shows cc and channel on top of oled
{
    oledPrint("CC: " + String(knob[activeKnob].midiCC));
    oledPrint("CH: " + String(knob[activeKnob].midiChannel+1), 90, 0, 0);
}

void showHome()
{
    oled.clear();
    showHeader();
    oledPrint("MIN: " + String(knob[activeKnob].min), 0, 3, 0);
    oledPrint("MAX: " + String(knob[activeKnob].max), 80, 3, 0);

    oledPrint(String(activeKnob), 55, 1, 3);
}

void showMin()
{
    oled.clear();
    showHeader();
    oledPrint(String(activeKnob), 55, 0, 3);
    oledPrint("MIN:" + String(knob[activeKnob].min), 0, 2, 1);
    oledPrint("MAX: " + String(knob[activeKnob].max), 80, 3, 0);
    oledPrint(String(activeKnob), 55, 0, 3);
}

void showMax()
{
    oled.clear();
    showHeader();
    oledPrint(String(activeKnob), 55, 0, 3);
    oledPrint("MIN: " + String(knob[activeKnob].min), 0, 3, 0);
    oledPrint("MAX:" + String(knob[activeKnob].max), 65, 2, 1);
    oledPrint(String(activeKnob), 55, 0, 3);
}

byte oldActiveKnob = 0;

void drawHome()
{
    if (activeKnob != oldActiveKnob || redrawOled)
    {
        oldActiveKnob = activeKnob;
        if (minButton.pressed)
            showMin();
        else if (maxButton.pressed)
            showMax();
        else
            showHome();
        redrawOled = false; // reset flag
    }

    if (minButton.fell)
        showMin();
    else if (minButton.rose)
        showHome();
    else if (maxButton.fell)
        showMax();
    else if (maxButton.rose)
        showHome();
}