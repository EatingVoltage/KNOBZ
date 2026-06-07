#include <Arduino.h>

void showHeader() // shows cc and channel on top of oled
{
    oledAt(0, 0, 0);
    oled.print(F("CC: "));
    oled.print(knob[activeKnob].midiCC);

    byte x = 90;
    if (knob[activeKnob].midiChannel + 1 < 10)
        x += 8;
    oledAt(x, 0, 0);
    oled.print(F("CH: "));
    oled.print(knob[activeKnob].midiChannel + 1);
}

void showHome()
{
    oled.clear();
    showHeader();
    oledAt(0, 3, 0);
    oled.print(F("MIN:"));
    oledPrintPad3(knob[activeKnob].min);
    oledAt(85, 3, 0);
    oled.print(F("MAX:"));
    oledPrintPad3(knob[activeKnob].max);

    byte x = 58;
    if (activeKnob < 10)
        x += 4;
    oledAt(x, 0, 3);
    oled.print(activeKnob + 1);
}

byte oldActiveKnob = 0;

void drawMin(byte col, byte style)
{
    oledAt(col, 2, style);
    oled.print(F("MIN:"));
    oledPrintPad3(knob[activeKnob].min);
}

void drawMax(byte col, byte style)
{
    oledAt(col, 2, style);
    oled.print(F("MAX:"));
    oledPrintPad3(knob[activeKnob].max);
}

void drawHome()
{
    if (activeKnob != oldActiveKnob || redrawOled)
    {
        oldActiveKnob = activeKnob;
        if (minButton.pressed)
            drawMin(0, 1);
        else if (maxButton.pressed)
            drawMax(74, 1);
        else
            showHome();
        redrawOled = false; // reset flag
    }

    else if (minButton.fell)
        drawMin(0, 1);
    else if (maxButton.fell)
        drawMax(74, 1);
    else if (minButton.rose || maxButton.rose)
        showHome();
}
