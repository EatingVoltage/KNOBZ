#include <Arduino.h>

void sendControllerMidi()
{
    for (byte i = 0; i < 37; i++)
    {
        if (knobs[i].hasNew)
        {
            byte val = map(knobs[i].val, 0, 127, knobs[i].min, knobs[i].max);
            sendMidiCC(knobs[i].midiCC, val, knobs[i].midiChannel);
        }
    }
}

void sendButtonMidi()
{
    for (byte i = 0; i < BUTTON_AMT; i++)
    {
        if (controllerButton[i].fell)
        {
            if (!menu.active)
            {
                sendMidiNoteOn(i, settings.midiDefaultVel, settings.midiChannel);
                delay(1);
                sendMidiNoteOn(i, 0, settings.midiChannel);
            }
            else 
            {
                menu.active = false;
                menu.editing = false;
                redrawOled = true;
            }
        }
    }
}

void forwardMidi()
{
    if (Serial1.available()) // very crude midi forwarding
    {
        while (Serial1.available())
        {
            // if midi tru enabled
            Serial1.write(Serial1.read());
            // write to usb midi
        }
        setPixel(0, 0, 0, 255);
        pixels.show();
    }
    // routeMidiUARTtoUSB();
    routeMidiUSBtoUART();
}