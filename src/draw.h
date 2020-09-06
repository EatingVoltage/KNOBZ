#include <Arduino.h>

void showHome()
{
    oled.clear();
    oledPrint("MIN: " + String(knobs[activeKnob].min), 0, 3, 0);
    oledPrint(" MAX: " + String(knobs[activeKnob].max), 75, 3, 0);
    oledPrint("CC: " + String(knobs[activeKnob].midiCC) + "          CH: " + String(knobs[activeKnob].midiChannel+1), 0, 0, 0);
    oledPrint(String(activeKnob), 55, 2, 3);
}

void showMin()
{
    oled.clear();
    oledPrint("MIN: " + String(knobs[activeKnob].min), 0, 2, 1);
    oledPrint(" MAX: " + String(knobs[activeKnob].max), 75, 3, 0);
    oledPrint("CC: " + String(knobs[activeKnob].midiCC) + "          CH: " + String(knobs[activeKnob].midiChannel+1), 0, 0, 0);
    // oledPrint(String(activeKnob), 55, 2, 3);
}

void showMax()
{
    oled.clear();
    oledPrint("MIN: " + String(knobs[activeKnob].min), 0, 3, 0);
    oledPrint("   MAX: " + String(knobs[activeKnob].max), 40, 2, 1);
    oledPrint("CC: " + String(knobs[activeKnob].midiCC) + "          CH: " + String(knobs[activeKnob].midiChannel+1), 0, 0, 0);
    // oledPrint(String(activeKnob), 55, 2, 3);
}

byte oldActiveKnob = 0;

void drawHome()
{
    if (activeKnob != oldActiveKnob || redrawOled)
    {
        oldActiveKnob = activeKnob;
        // if (!menu.pos)
        // {
            if (minButton.pressed)
                showMin();
            else if (maxButton.pressed)
                showMax();
            else
                showHome();
            redrawOled = false; // reset flag
        // }
    }

    if (minButton.fell)
        showMin();
    else if (minButton.rose)
        showHome();
    else if (maxButton.fell)
        showMax();
    else if (maxButton.rose)
        showHome();

    // else showHome();

    // uiState++;
    // if (uiState > 2)
    //     uiState = 0;
    // switch (uiState)
    // {
    // case 0:
    // showHome();
    //     break;

    // case 1:
    //     showMin();
    //     break;

    // case 2:
    //     showMax();
    //     break;

    // default:
    //     break;
    // }
}