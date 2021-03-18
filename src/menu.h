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
        oledPrint("---------MENU--------", 0, 0, 0);
        oledPrint("<-       ok       ->", 0, 3, 0);

        switch (menu.pos)
        {
        case 0:
            oledPrint("Set Knob " + String(menu.editingKnob) + " CC." + String(knob[menu.editingKnob].midiCC), 0, 1, 1);
            break;

        case 1:
            oledPrint("Set Knob " + String(menu.editingKnob) + " CH.", 0, 1, 1);
            break;

        case 2:
            oledPrint("Set All Channels", 0, 1, 1);
            break;

        case 3:
            oledPrint("Clear All Knobs", 0, 1, 1);
            break;

        case 4:
            oledPrint("Clear All MIN/MAX", 0, 1, 1);
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
        oledPrint("-       back       +", 0, 3, 0);
        // oledPrint("back", 60, 3, 0);
        // oledPrint("+", 105, 3•, 0);

        switch (menu.pos)
        {
        case 0:
            oledPrint("Knob " + String(menu.editingKnob) + " CC: " + String(knob[menu.editingKnob].midiCC), 0, 1, 1);
            for (byte i = 0; i < KNOB_AMT; i++)
            {
                if (i != menu.editingKnob)
                {
                    if ((knob[menu.editingKnob].midiCC == knob[i].midiCC) && (knob[menu.editingKnob].midiChannel == knob[i].midiChannel))
                    {
                        // menu.ccConflict = true;
                        oledPrint("! CC/CH taken !", 25, 0, 0);
                    }
                }
            }

            break;
        case 1:
            oledPrint("Knob " + String(menu.editingKnob) + " CH: " + String(knob[menu.editingKnob].midiChannel), 0, 1, 1);
            for (byte i = 0; i < KNOB_AMT; i++) // check for conflicting knob
            {
                if (i != menu.editingKnob)
                {
                    if ((knob[menu.editingKnob].midiCC == knob[i].midiCC) && (knob[menu.editingKnob].midiChannel == knob[i].midiChannel))
                    {
                        oledPrint("! CC/CH taken !", 25, 0, 0);
                    }
                }
            }
            break;
        case 2:
            // menu.currentVal = 0;
            oledPrint("set all ch: " + String(menu.currentVal), 0, 1, 1);
            break;
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
                if (minButton.fell || (minButton.didHold && minButton.pressed))
                {
                    menu.currentVal = knob[menu.editingKnob].midiCC;
                    if (menu.currentVal == 0)
                        menu.currentVal = 127;
                    else
                        menu.currentVal -= 1;
                    knob[menu.editingKnob].midiCC = menu.currentVal;
                    drawMenu();
                }
                if (maxButton.fell || (maxButton.didHold && maxButton.pressed))
                {
                    byte x = knob[menu.editingKnob].midiCC + 1;
                    if (x >= 128)
                        x = 0;
                    knob[menu.editingKnob].midiCC = x;
                    drawMenu();
                }
                break;

            case 1: // Knob Channel editing
                if (minButton.fell || (minButton.didHold && minButton.pressed))
                {
                    menu.currentVal = knob[menu.editingKnob].midiChannel;
                    if (menu.currentVal == 0)
                        menu.currentVal = 15;
                    else
                        menu.currentVal -= 1;
                    knob[menu.editingKnob].midiChannel = menu.currentVal;
                    drawMenu();
                }
                if (maxButton.fell || (maxButton.didHold && maxButton.pressed))
                {
                    menu.currentVal = knob[menu.editingKnob].midiChannel + 1;
                    if (menu.currentVal >= 15)
                        menu.currentVal = 0;
                    knob[menu.editingKnob].midiChannel = menu.currentVal;
                    drawMenu();
                }
                break;

            case 2: // set knob channels
                if (minButton.fell)
                {
                    menu.currentVal = knob[menu.editingKnob].midiChannel;
                    if (menu.currentVal == 0)
                        menu.currentVal = 15;
                    else
                        menu.currentVal -= 1;
                    for (byte i = 0; i < KNOB_AMT; i++)
                    {
                        knob[i].midiChannel = menu.currentVal;
                    }

                    drawMenu();
                }
                if (maxButton.fell)
                {
                    menu.currentVal = knob[menu.editingKnob].midiChannel + 1;
                    if (menu.currentVal >= 15)
                        menu.currentVal = 0;
                    for (byte i = 0; i < KNOB_AMT; i++)
                    {
                        knob[i].midiChannel = menu.currentVal;
                    }
                    drawMenu();
                }

                break;

            case 3: // clear all
                    // oled.clear();
                    // oledPrint("reset all?", 0, 1, 1);
                    // oledPrint("ok", 50, 3, 0);
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

                menu.editing = false;
                drawMenu();
                //     }
                //     if(minButton.fell)
                //     {
                //         done = true;
                //     }
                // }
                break;

            case 4: // reset min/max
                    // oled.clear();
                    // oledPrint("reset min/max?", 0, 1, 1);
                    // oledPrint("ok", 50, 3, 0);
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
                    knob[i].midiChannel = 1;
                    knob[i].midiCC = i;
                    knob[i].max = 127;
                    knob[i].min = 0;
                }
                menu.editing = false;
                drawMenu();
                //     }
                //     if(minButton.fell)
                //     {
                //         done = true;
                //     }
                // }
                break;

            case 5: // save controller state
                oledPrint("Select Save Slot", 0, 1, 1);
                s = 100; // 0-3
                done = false;
                while (!done) // Todo: make not blockingn
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
                    updateButtons(millis());
                    if (modeButton.fell)
                        done = true;
                }

                // save the data
                if (s != 100) // validation
                {
                    oled.clear();
                    oledPrint("saving to slot " + String(s + 1), 0, 1, 1);
                    saveConfig(s);
                    myDelay(500);
                }
                break;

            case 6: // load
                oledPrint("Select Load Slot", 0, 1, 1);
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

                    updateMidi();

                    myDelay(50); // easy, boy
                    if (modeButton.fell)
                        done = true;
                }
                // load data
                if (s != 100) // omit if aborted
                {
                    oled.clear();
                    oledPrint("loading slot " + String(s + 1), 0, 1, 1);
                    loadConfig(s);
                    myDelay(500);
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