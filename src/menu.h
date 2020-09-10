#include <Arduino.h>

#define MENU_ITEMS 7 // number of menu items

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

void drawMenu()
{
    menu.t0 = millis(); // reset menu timer on every change

    oled.clear();
    // oledPrint(String(menu.pos), 0, 1, 0);

    if (!menu.editing) // selecting menu item
    {
        oledPrint("---------MENU-------" + String(menu.pos), 0, 0, 0);
        oledPrint("<-", 0, 3, 0);
        oledPrint("ok", 63, 3, 0);
        oledPrint("->", 115, 3, 0);

        switch (menu.pos)
        {
        case 0:
            oledPrint("Edit Knob " + String(menu.editingKnob) + " CC " + String(knobs[menu.editingKnob].midiCC), 0, 1, 1);
            break;

        case 1:
            oledPrint("Edit Knob " + String(menu.editingKnob) + " Channel", 0, 1, 1);
            break;

        case 2:
            oledPrint("set all Channels", 0, 1, 1);
            break;

        case 3:
            oledPrint("reset all knobs", 0, 1, 1);
            break;

        case 4: 
            oledPrint("reset all min/max", 0, 1, 1);
            break;

        case 5:
            oledPrint("Save Config", 20, 1, 1);
            break;

        case 6:
            oledPrint("Load Config", 20, 1, 1);
            break;

        default:
            break;
        }
    }

    else // editing menu item
    {
        oledPrint("---------EDIT---------", 0, 0, 0);
        oledPrint("-", 0, 3, 0);
        oledPrint("back", 60, 3, 0);
        oledPrint("+", 105, 3, 0);

        switch (menu.pos)
        {
        case 0:
            oledPrint("Knob " + String(menu.editingKnob) + " CC: " + String(knobs[menu.editingKnob].midiCC), 0, 1, 1);
            for (byte i = 0; i < KNOB_AMT; i++)
            {
                if (i != menu.editingKnob)
                {
                    if ((knobs[menu.editingKnob].midiCC == knobs[i].midiCC) && (knobs[menu.editingKnob].midiChannel == knobs[i].midiChannel))
                    {
                        // menu.ccConflict = true;
                        oledPrint("! CC/CH taken !", 25, 3, 0);
                    }
                }
            }

            break;
        case 1:
            oledPrint("Knob " + String(menu.editingKnob) + " CH: " + String(knobs[menu.editingKnob].midiChannel), 0, 1, 1);
            for (byte i = 0; i < KNOB_AMT; i++) // check for conflicting knobs
            {
                if (i != menu.editingKnob)
                {
                    if ((knobs[menu.editingKnob].midiCC == knobs[i].midiCC) && (knobs[menu.editingKnob].midiChannel == knobs[i].midiChannel))
                    {
                        oledPrint("! CC/CH taken !", 25, 3, 0);
                    }
                }
            }
            break;
        // case 2:
            // menu.currentVal = 0;
            // oledPrint("set all channels: " + menu.currentVal, 0, 1, 1);
            // break;
        // case 3:
            // oledPrint("clear all?", 0, 1, 1);
            // break;
        // case 4: // 
            // oledPrint("reset min/max?", 0, 1, 1);
            // break;

        // case 5: // save
            // break;

        // case 6: // load
            // break;

        default:
            break;
        }
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
                if (minButton.fell)
                {
                    byte x = knobs[menu.editingKnob].midiCC;
                    if (x == 0)
                        x = 127;
                    else
                        x -= 1;
                    knobs[menu.editingKnob].midiCC = x;
                    drawMenu();
                }
                if (maxButton.fell)
                {
                    byte x = knobs[menu.editingKnob].midiCC + 1;
                    if (x >= 128)
                        x = 0;
                    knobs[menu.editingKnob].midiCC = x;
                    drawMenu();
                }
                break;

            case 1: // Knob Channel editing
                if (minButton.fell)
                {
                    byte x = knobs[menu.editingKnob].midiChannel;
                    if (x == 0)
                        x = 15;
                    else
                        x -= 1;
                    knobs[menu.editingKnob].midiChannel = x;
                    drawMenu();
                }
                if (maxButton.fell)
                {
                    byte x = knobs[menu.editingKnob].midiChannel + 1;
                    if (x >= 15)
                        x = 0;
                    knobs[menu.editingKnob].midiChannel = x;
                    drawMenu();
                }
                break;

            case 2: // set knob channels
                break;

            case 3: // clear all
                break;
            
            case 4: // reset min/max
                break;

            case 5: // save controller state
                oledPrint("select save slot", 0, 1, 1);
                s = 100; // 0-3
                done = false;
                while (!done)
                {
                    inputValues = shiftInUpdate();
                    updateButtons(millis());
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
                    if (modeButton.fell)
                        done = true;
                }

                // save the data
                if (s != 100) // validation
                {
                    oled.clear();
                    oledPrint("saving to slot " + String(s + 1), 0, 1, 1);
                    saveConfig(s);
                    delay(500);
                }
                break;

            case 6: // load
                oledPrint("select load slot", 0, 1, 1);
                s = 100; // 0-3
                done = false;
                while (!done)
                {
                    inputValues = shiftInUpdate();
                    updateButtons(millis());
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
                    delay(50); // easy, boy
                    if (modeButton.fell)
                        done = true;
                }
                // load data
                if (s != 100) // omit if aborted
                {
                    oled.clear();
                    oledPrint("loading slot " + String(s + 1), 0, 1, 1);
                    loadConfig(s);
                    delay(500);
                }
                break;
            } // end of switch(editing menu pos)

            if (modeButton.fell) // exit condition - back to selection menu
            {
                menu.editing = false;
                drawMenu();
            }

            // menu timeout
            if (millis() - menu.t0 > MENU_TIMEOUT)
            {
                menu.active = false;
                menu.editing = false;
                redrawOled = true;
            }
        }
    }
}