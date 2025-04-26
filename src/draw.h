#include <Arduino.h>

String fNum(byte _msg, byte targetLen = 2) // adds " " spaces to number
{
    String res = String(_msg);
    while (res.length() < 3)
        res = " " + res;
    return res;
}

void showHeader() // shows cc and channel on top of oled
{
    oledPrint("CC: " + String(knob[activeKnob].midiCC));

    byte x = 90;
    if (knob[activeKnob].midiChannel + 1 < 10)
        x += 8;
    oledPrint("CH: " + String(knob[activeKnob].midiChannel + 1), x, 0, 0);
    // oledPrint("CH: " + String(knob[activeKnob].midiChannel+1), 95, 0, 0);
}

void showHome()
{
    oled.clear();
    showHeader();
    oledPrint("MIN:" + fNum(knob[activeKnob].min, 3), 0, 3, 0);
    oledPrint("MAX:" + fNum(knob[activeKnob].max, 3), 85, 3, 0);

    byte x = 58;
    if (activeKnob < 10)
        x += 4;
    oledPrint(String(activeKnob+1), x, 0, 3);
}

byte oldActiveKnob = 0;

void drawHome()
{
    if (activeKnob != oldActiveKnob || redrawOled)
    {
        oldActiveKnob = activeKnob;
        if (minButton.pressed)
            oledPrint("MIN:" + fNum(knob[activeKnob].min, 3), 0, 2, 1);
        else if (maxButton.pressed)
            oledPrint("MAX:" + fNum(knob[activeKnob].max, 3), 74, 2, 1);
        else
            showHome();
        redrawOled = false; // reset flag
    }

    else if (minButton.fell)
        oledPrint("MIN:" + fNum(knob[activeKnob].min, 3), 0, 2, 1);
    else if (maxButton.fell)
        oledPrint("MAX:" + fNum(knob[activeKnob].max, 3), 74, 2, 1);
    else if (minButton.rose || maxButton.rose)
        showHome();
}