#include <Arduino.h>

struct menu_t
{
    byte pos = 0;
    bool active = false;
    bool editing = false;
    byte items = 7; // number of menu items
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

    if (!menu.editing)
    {
        oledPrint("--------MENU--------" + String(menu.pos + 1), 0, 0, 0);
        oledPrint("<-", 0, 3, 0);
        oledPrint("ok", 60, 3, 0);
        oledPrint("->", 105, 3, 0);

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
        oledPrint("+", 105, 3, 0);

        switch (menu.pos)
        {
        case 0:
            oledPrint("Knob " + String(menu.editingKnob) + " CC:" + String(knobs[menu.editingKnob].midiCC), 0, 1, 1);
            break;
        case 1:
            oledPrint("Knob " + String(menu.editingKnob) + " CH:" + String(knobs[menu.editingKnob].midiChannel + 1), 0, 1, 1);
            // oledPrint("Knob " + String(menu.editingKnob) + " CH:", 0, 1, 1);
            break;
        case 2:
            oledPrint("set all knobs to ", 0, 1, 1);
            break;
        case 3:
            oledPrint("Knobsmooth. :", 0, 1, 1);
            break;
        case 4:
            oledPrint("select Save Slot", 0, 1, 1);
            byte saveSlot = 100; // 0-3
            bool done = false;
            while (!done)
            {
                inputValues = shiftInUpdate();
                updateButtons(millis());
                if (controllerButton[0].fell || controllerButton[1].fell || controllerButton[2].fell || controllerButton[1].fell)
                {
                    for (byte i = 0; i < 4; i++)
                    {
                        if (controllerButton[i].fell)
                        {
                            saveSlot = i;
                        }
                    }
                    done = true;
                }
                delay(50); // easy, buoy
            }
            if (saveSlot != 100)
            {
                oledPrint("save slot" + String(saveSlot), 0, 0, 1);
                delay(1000);
            }
            // save the data
            break;

        case 5:
            oledPrint("select Load Slot", 0, 1, 1);
            byte loadSlot = 100; // 0-3
            done = false;
            while (!done)
            {
                inputValues = shiftInUpdate();
                updateButtons(millis());
                if (controllerButton[0].fell || controllerButton[1].fell || controllerButton[2].fell || controllerButton[1].fell)
                {
                    for (byte i = 0; i < 4; i++)
                    {
                        if (controllerButton[i].fell)
                        {
                            loadSlot = i;
                        }
                    }
                    done = true;
                }
                delay(50); // easy, boy
                if (modeButton.fell)
                    done = true;
            }
            // load data
            if (loadSlot != 100)
            {
                oledPrint("load slot" + String(loadSlot), 0, 0, 1);
                delay(1000);
            }
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
            if (!menu.editing)
            {
                if (maxButton.fell) // navigate
                {
                    menu.pos++;
                    if (menu.pos >= menu.items - 1)
                        menu.pos = 0;
                    drawMenu();
                }

                if (minButton.fell) // navigate
                {
                    menu.pos--;
                    if (menu.pos == 255)
                        menu.pos = menu.items - 1;
                    drawMenu();
                }

                if (modeButton.fell)
                {
                    menu.editing = true;
                    drawMenu();
                }
            }
        }

        else // editing menu item
        {
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

                break;
            case 3: // smoothing editing

                break;
            case 4: // CC editing

                break;
            case 5: // CC editing

                break;

            default:
                break;
            }

            if (modeButton.fell) // exit condition
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
        }
    }
}