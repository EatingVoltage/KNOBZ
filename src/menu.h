#include <Arduino.h>

struct menu_t
{
    byte pos = 0;
    bool active = false;
    bool editing = false;
    byte items = 6; // number of menu items
    long t0 = 0;
    byte editingKnob;
    byte currentVal = 0; // temp space for numerical menu items
};

menu_t menu;

void drawMenu()
{
    menu.t0 = millis(); // reset menu timer on every change

    oled.clear();

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
            oledPrint("Set global Channel", 0, 1, 1);
            break;

        case 3:
            oledPrint("Knob " + String(menu.editingKnob) + " smoothing", 0, 1, 1);
            break;

        case 4:
            oledPrint("Save Config", 0, 1, 1);
            break;
        case 5:
            oledPrint("Load Config", 0, 1, 1);
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
        oledPrint("+", 120, 3, 0);

        switch (menu.pos)
        {
        case 0:
            oledPrint("Knob " + String(menu.editingKnob) + " CC: " + String(knobs[menu.editingKnob].midiCC), 0, 1, 1);
            for (byte i = 0; i < 37; i++)
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
            oledPrint("Knob " + String(menu.editingKnob) + " CH: " + String(knobs[menu.editingKnob].midiChannel + 1), 0, 1, 1);
            for (byte i = 0; i < 37; i++) // check for conflicting knobs
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
        case 2:
            oledPrint("set all knobs to ", 0, 1, 1);
            break;
        case 3:
            oledPrint("Knobsmooth. :", 0, 1, 1);
            break;
        case 4: // save

            break;

        case 5: // load

            break;

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
                if (menu.pos == menu.items - 1)
                    menu.pos = 0;
                else
                    menu.pos++;
                drawMenu();
            }

            if (minButton.fell) // navigate
            {
                if (menu.pos == 0)
                    menu.pos = menu.items - 1;
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
            case 0: // CC editing
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

            case 1: // Channel editing
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

            case 2: // global Channel editing
                oledPrint("work in progress");
                break;

            case 3: // smoothing editing
                oledPrint("work in progress");
                break;

            case 4: // save controller state
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
                    delay(50); // easy, buoy
                }

                // save the data
                if (s != 100) // validation
                {
                    oled.clear();
                    oledPrint("saving to slot " + String(s + 1), 20, 0, 3);
                    saveConfig(s);
                    delay(500);
                    oledPrint("done.", 80, 3, 0);
                    delay(300);
                }
                break;

            case 5: // load
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
                    oledPrint("loading from slot " + String(s + 1), 10, 0, 3);
                    loadConfig(s);
                    delay(500);
                    oledPrint("done.", 80, 3, 0);
                    delay(300);
                }
                break;
            } // end of switch(editing menu pos)

            if (modeButton.fell) // exit condition
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