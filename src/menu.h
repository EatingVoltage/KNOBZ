#include <Arduino.h>

#define MENU_ITEMS 8 // number of menu items

struct menu_t
{
    byte pos = 0;
    bool active = false;
    bool editing = false;
    long t0 = 0;
    byte editingKnob;
    byte currentVal = 0; // temp space for numerical menu items
};

menu_t menu;

void checkConflict()
{
    for (byte i = 0; i < KNOB_AMT; i++)
    {
        if ((knob[menu.editingKnob].midiCC == knob[i].midiCC) && (knob[menu.editingKnob].midiChannel == knob[i].midiChannel) && i != menu.editingKnob)
        {
            // menu.ccConflict = true;
            oledPrint(F("! CC/CH taken !"), 25, 0, 0);
        }
    }
}

// redraws ONLY the editing value line (cases 0-3) without an oled.clear(), so
// nudging a value doesn't flicker the whole screen. for cc/ch the header is
// reprinted in place (overwrites identical pixels, no blank) which also wipes a
// stale "CC/CH taken" warning; clearToEOL erases leftover digits (127 -> 9).
void drawEditValue()
{
    menu.t0 = millis(); // reset menu timer on every change

    switch (menu.pos)
    {
    case 0:
        oledPrint(F("---------EDIT---------"), 0, 0, 0);
        oledAt(0, 1, 1);
        oled.print(F("Knob "));
        oled.print(menu.editingKnob + 1);
        oled.print(F(" CC: "));
        oled.print(knob[menu.editingKnob].midiCC);
        oled.clearToEOL();
        checkConflict();
        break;
    case 1:
        oledPrint(F("---------EDIT---------"), 0, 0, 0);
        oledAt(0, 1, 1);
        oled.print(F("Knob "));
        oled.print(menu.editingKnob + 1);
        oled.print(F(" CH: "));
        oled.print(knob[menu.editingKnob].midiChannel + 1);
        oled.clearToEOL();
        checkConflict();
        break;
    case 2:
        oledAt(0, 1, 1);
        oled.print(F("set all ch: "));
        oled.print(menu.currentVal);
        oled.clearToEOL();
        break;
    case 3:
        oledAt(0, 1, 1);
        oled.print(F("Button CH: "));
        oled.print(settings.midiChannel + 1);
        oled.clearToEOL();
        break;
    default:
        break;
    }
}

void drawMenu()
{
    menu.t0 = millis(); // reset menu timer on every change

    oled.clear();
    // oledPrint(String(menu.pos), 0, 1, 0);

    if (!menu.editing) // selecting menu item
    {
        oledPrint(F("---------MENU--------"), 0, 0, 0);
        oledPrint(F("<-       ok       ->"), 0, 3, 0);

        switch (menu.pos)
        {
        case 0:
            oledAt(0, 1, 1);
            oled.print(F("Set Knob "));
            oled.print(menu.editingKnob + 1);
            oled.print(F(" CC."));
            break;

        case 1:
            oledAt(0, 1, 1);
            oled.print(F("Set Knob "));
            oled.print(menu.editingKnob + 1);
            oled.print(F(" CH."));
            break;

        case 2:
            oledPrint(F("Set all Channels"), 0, 1, 1);
            break;

        case 3:
            oledPrint(F("Set Button CH."), 0, 1, 1);
            break;

        case 4:
            oledPrint(F("Clear all Knobs"), 0, 1, 1);
            break;

        case 5:
            oledPrint(F("Clear all MIN/MAX"), 0, 1, 1);
            break;

        case 6:
            oledPrint(F("Save Config"), 20, 1, 1);
            break;

        case 7:
            oledPrint(F("Load Config"), 20, 1, 1);
            break;

        default:
            break;
        }
    }

    else // editing menu item
    {
        oledPrint(F("---------EDIT---------"), 0, 0, 0);

        if (menu.pos == 6) // selecting save slot
            oledPrint(F("         back         "), 0, 3, 0);
        else
        {
            oledPrint(F("-        set        +"), 0, 3, 0);
            drawEditValue(); // value line (cases 0-3); shared with the flicker-free nudge path
        }
    }
}

// draws the name-entry field: full NAME_LEN width, x-centered, with a cursor
// marker beneath the active position. show=false blanks the name (blink frame).
// redraws in place (no oled.clear()) to avoid flicker on every char/blink frame.
// caller clears the screen once before the first draw. the fixed-width name field
// overwrites itself; only the cursor row is region-cleared so the marker can move.
void drawNameEntry(byte cursor, bool show)
{
    oledPrint(F("name your preset:"), 0, 0, 0); // static header, redrawn in place
    byte col = (128 - NAME_LEN * 8) / 2;        // fixed-width field so the marker stays aligned
    oledAt(col, 1, 1);                          // big field on rows 1-2
    for (byte i = 0; i < NAME_LEN; i++)
        oled.print(show ? presetName[i] : ' ');
    oled.clear(col, col + NAME_LEN * 8 - 1, 3, 3); // wipe old marker
    oledAt(col + cursor * 8, 3, 0);                 // cursor marker on row 3
    oled.print(F("^"));
}

// name-entry alphabet: blank (bottom) | A-Z | 0-9 | symbols | blank (top).
// both knob extremes give a blank so a position is easy to clear.
const char charset[] PROGMEM = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+&#!?.*=/ ";
#define CHARSET_LEN 48 // glyph count incl. both blank ends (= sizeof(charset) - 1)

// letter-input routine. starts blank. any knob sets the char at the cursor
// (knob 0-127 -> charset); min/max move the cursor; hold mode >=2s to confirm.
// no abort - you commit to a name (menu timeout is the only passive exit upstream).
void enterName()
{
    for (byte i = 0; i < NAME_LEN; i++)
        presetName[i] = ' ';
    byte cursor = 0;
    long holdT0 = 0;
    long blinkT0 = 0;
    bool show = true;
    oled.clear(); // clear once; drawNameEntry redraws in place afterwards
    drawNameEntry(cursor, show);

    while (true)
    {
        inputValues = shiftInUpdate();
        updateButtons();
        muxRead();
        updateKnobs();
        updateMidi();

        bool dirty = false;

        for (byte i = 0; i < KNOB_AMT; i++) // any knob writes the char at the cursor
        {
            if (knob[i].hasNew)
            {
                byte idx = map(knob[i].getVal(), 0, 127, 0, CHARSET_LEN - 1);
                presetName[cursor] = pgm_read_byte(&charset[idx]);
                dirty = true;
            }
        }

        if (minButton.fell && cursor > 0) // cursor left
        {
            cursor--;
            dirty = true;
        }
        if (maxButton.fell && cursor < NAME_LEN - 1) // cursor right
        {
            cursor++;
            dirty = true;
        }

        if (modeButton.pressed) // hold set -> blink, confirm after 2s
        {
            if (holdT0 == 0)
            {
                holdT0 = millis();
                blinkT0 = millis();
                show = false;
                dirty = true;
            }
            if (millis() - holdT0 > 2000)
                break; // confirm
            if (millis() - blinkT0 > 150)
            {
                blinkT0 = millis();
                show = !show;
                dirty = true;
            }
        }
        else if (holdT0 != 0) // released before 2s -> keep editing
        {
            holdT0 = 0;
            show = true;
            dirty = true;
        }

        if (dirty)
            drawNameEntry(cursor, show);
    }
}

void updateMenu()
{
    if (!(menu.active)) // activate menu
    {
        if (modeButton.fell)
        {
            menu.active = true;
            menu.editingKnob = activeKnob;
            drawMenu();
        }
    }

    else
    {
        if (!menu.editing) // selecting menu item
        {
            // check for input
            if (maxButton.fell) // navigate
            {
                if (menu.pos == MENU_ITEMS - 1)
                    menu.pos = 0;
                else
                    menu.pos++;
                drawMenu();
            }

            if (minButton.fell) // navigate
            {
                if (menu.pos == 0)
                    menu.pos = MENU_ITEMS - 1;
                else
                    menu.pos--;
                drawMenu();
            }

            if (modeButton.fell)
            {
                menu.editing = true;
                drawMenu();
            }
        }

        else // editing menu item
        {
            bool done;
            byte s;

            switch (menu.pos)
            {
            case 0: // Knob CC editing
                if (minButton.fell || (minButton.didHold && minButton.pressed))
                {
                    menu.currentVal = knob[menu.editingKnob].midiCC;
                    if (menu.currentVal == 0)
                        menu.currentVal = 127;
                    else
                        menu.currentVal -= 1;
                    knob[menu.editingKnob].midiCC = menu.currentVal;
                    drawEditValue();
                }
                if (maxButton.fell || (maxButton.didHold && maxButton.pressed))
                {
                    menu.currentVal = knob[menu.editingKnob].midiCC + 1;
                    if (menu.currentVal >= 128)
                        menu.currentVal = 0;
                    knob[menu.editingKnob].midiCC = menu.currentVal;
                    drawEditValue();
                }
                if (knob[menu.editingKnob].hasNew) // can edit menuValue with knob
                {
                    menu.currentVal = knob[menu.editingKnob].getVal();
                    knob[menu.editingKnob].midiCC = knob[menu.editingKnob].getVal();
                    drawEditValue();
                }

                break;

            case 1: // Knob Channel editing
                menu.currentVal = knob[menu.editingKnob].midiChannel + 1;
                if (minButton.fell || (minButton.didHold && minButton.pressed))
                {
                    menu.currentVal--;
                    if (menu.currentVal == 0)
                        menu.currentVal = 16;
                    knob[menu.editingKnob].midiChannel = menu.currentVal - 1;
                    drawEditValue();
                }
                if (maxButton.fell || (maxButton.didHold && maxButton.pressed))
                {
                    // menu.currentVal = knob[menu.editingKnob].midiChannel + 1;
                    menu.currentVal++;
                    if (menu.currentVal == 17)
                        menu.currentVal = 1;
                    knob[menu.editingKnob].midiChannel = menu.currentVal - 1;
                    drawEditValue();
                }
                if (knob[menu.editingKnob].hasNew) // can edit menuValue with knob
                {
                    menu.currentVal = knob[menu.editingKnob].getVal();
                    knob[menu.editingKnob].midiChannel = map(knob[menu.editingKnob].getVal(), 0, 127, 0, 15);
                    drawEditValue();
                }
                break;

            case 2:                                                       // set all knobs channels
                menu.currentVal = knob[menu.editingKnob].midiChannel + 1; // get recent data
                if (minButton.fell || (minButton.didHold && minButton.pressed))
                {
                    menu.currentVal--;
                    if (menu.currentVal == 0)
                        menu.currentVal = 16;
                    for (byte i = 0; i < KNOB_AMT; i++)
                    {
                        knob[i].midiChannel = menu.currentVal - 1; // save channel as 0-15, show and send as 1-16
                    }

                    drawEditValue();
                }
                if (maxButton.fell || (maxButton.didHold && maxButton.pressed))
                {
                    menu.currentVal++;
                    if (menu.currentVal == 17)
                        menu.currentVal = 1;

                    // knob[menu.editingKnob].midiChannel = menu.currentVal - 1;
                    for (byte i = 0; i < KNOB_AMT; i++)
                    {
                        knob[i].midiChannel = menu.currentVal - 1; // save channel as 0-15, show and send as 1-16
                    }
                    drawEditValue();
                }

                break;

            case 3: // set note-button channel (mirrors "set all channels")
                menu.currentVal = settings.midiChannel + 1; // get recent data, shown 1-16
                if (minButton.fell || (minButton.didHold && minButton.pressed))
                {
                    menu.currentVal--;
                    if (menu.currentVal == 0)
                        menu.currentVal = 16;
                    settings.midiChannel = menu.currentVal - 1; // store 0-15
                    drawEditValue();
                }
                if (maxButton.fell || (maxButton.didHold && maxButton.pressed))
                {
                    menu.currentVal++;
                    if (menu.currentVal == 17)
                        menu.currentVal = 1;
                    settings.midiChannel = menu.currentVal - 1; // store 0-15
                    drawEditValue();
                }
                break;

            case 4: // clear all
                // oled.clear();
                // oledPrint("reset all?", 0, 1, 1);
                // done = false;
                // while(!done)
                // {
                //     inputValues = shiftInUpdate();
                //     updateButtons(millis());
                //     if (maxButton.fell)
                //     {
                //         done = true;
                for (byte i = 0; i < KNOB_AMT; i++)
                {
                    knob[i].midiChannel = settings.midiChannel;
                    knob[i].midiCC = i;
                    knob[i].max = 127;
                    knob[i].min = 0;
                }
                oledPrint(F(" done"), 40, 2, 1);
                myDelay(500);
                menu.editing = false;
                drawMenu();
                break;

            case 5: // reset min/max
                for (byte i = 0; i < KNOB_AMT; i++)
                {
                    knob[i].midiChannel = 0;
                    knob[i].midiCC = i+1;
                    knob[i].max = 127;
                    knob[i].min = 0;
                }
                oledPrint(F(" done"), 40, 2, 1);
                myDelay(500);
                menu.editing = false;
                drawMenu();
                break;

            case 6: // save controller state
                oledPrint(F("Select Save Slot"), 0, 1, 1);
                s = 100; // 0-3
                done = false;
                while (!done)
                {
                    // get save slot
                    if (controllerButton[0].fell || controllerButton[1].fell || controllerButton[2].fell || controllerButton[3].fell)
                    {
                        for (byte i = 0; i < 4; i++)
                        {
                            if (controllerButton[i].fell)
                            {
                                s = i;
                            }
                        }
                        done = true;
                    }

                    updateMidi();

                    // exit condition
                    inputValues = shiftInUpdate();
                    updateButtons();
                    if (modeButton.fell)
                        done = true;
                }

                // save the data
                if (s != 100) // validation
                {
                    enterName(); // name the preset before writing (fills presetName)
                    oled.clear();
                    oledAt(0, 1, 1);
                    oled.print(F("saving to slot "));
                    oled.print(s + 1);
                    saveConfig(s); // writes knob config + button channel + name
                    myDelay(500);
                    menu.editing = false;
                    menu.active = false;
                    redrawOled = true;
                }
                break;

            case 7: // load
                oledPrint(F("Select Load Slot"), 0, 1, 1);
                s = 100; // 0-3
                done = false;
                while (!done)
                {
                    inputValues = shiftInUpdate();
                    updateButtons();
                    if (controllerButton[0].fell || controllerButton[1].fell || controllerButton[2].fell || controllerButton[3].fell)
                    {
                        for (byte i = 0; i < 4; i++)
                        {
                            if (controllerButton[i].fell)
                            {
                                s = i;
                            }
                        }
                        done = true;
                    }

                    updateMidi();

                    myDelay(50); // easy, boy
                    if (modeButton.fell)
                        done = true;
                }
                // load data
                if (s != 100) // omit if aborted
                {
                    loadConfig(s); // fills presetName before we show it
                    oled.clear();
                    oledPrint(F("Loading Slot "), 16, 0, 0);
                    oled.print(s + 1);
                    drawNameCentered(2, true); // name x-centered, letter-by-letter
                    myDelay(300);
                    menu.editing = false;
                    menu.active = false;
                    redrawOled = true;
                }
                break;
            } // end of switch(editing menu pos)

            if (modeButton.fell) // exit condition - back to selection menu
            {
                menu.editing = false;
                drawMenu();
            }
        }
        // menu timeout
        if (millis() - menu.t0 > MENU_TIMEOUT)
        {
            menu.active = false;
            menu.editing = false;
            redrawOled = true;
            // Serial.println("exiting menu.");
        }
    }
}