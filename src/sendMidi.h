#include <Arduino.h>

void sendControllerMidi()
{
    for (byte i = 0; i < KNOB_AMT; i++)
    {
        if (knob[i].hasNew)
        {
            byte val = map(knob[i].getVal(), 0, 127, knob[i].min, knob[i].max);
            sendMidiCC(knob[i].midiCC, val, knob[i].midiChannel);
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
                sendMidiNoteOn(i, MIDI_DEFAULT_VEL, settings.midiChannel);
                // delayMicroseconds(100); // to test. maybe can reduce time here
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

