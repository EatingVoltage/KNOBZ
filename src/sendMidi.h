#include <Arduino.h>

void sendControllerMidi()
{
    for (byte i = 0; i < KNOB_AMT; i++)
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
                sendMidiNoteOn(i, MIDI_DEFAULT_VEL, settings.midiChannel);
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

