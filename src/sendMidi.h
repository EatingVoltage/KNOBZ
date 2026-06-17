#include <Arduino.h>

void sendControllerMidi()
{
    for (byte i = 0; i < KNOB_AMT; i++)
    {
        if (knob[i].hasNew)
        {
            byte val = 0;
            if (minButton.pressed)
                val = knob[i].min; //
            else if (maxButton.pressed)
                val = knob[i].max; // dont scale if setting min or max
            else
                val = map(knob[i].getVal(), 0, 127, knob[i].min, knob[i].max); // default case
            // Serial.println(val);
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
            if (menu.active)
            {
                menu.active = false;
                menu.editing = false;
                redrawOled = true;
            }
            else
            {
                sendMidiNoteOn(i, MIDI_DEFAULT_VEL, settings.midiChannel);
            }
        }
        else if (controllerButton[i].rose)
        {
            if (menu.active)
            {
                menu.active = false;
                menu.editing = false;
                redrawOled = true;
            }
            else
                sendMidiNoteOn(i, 0, settings.midiChannel);
        }
    }

    // lidar sensor toggle switch: latching note, sent on the button channel.
    // ignores menu state (it is a hardware mode switch, not a UI button).
    // fell = switch to sensor-ON position -> note held; rose = sensor-OFF -> note off.
    if (sensorSwitch.fell)
        sendMidiNoteOn(SENSOR_SWITCH_NOTE, MIDI_DEFAULT_VEL, settings.midiChannel);
    else if (sensorSwitch.rose)
        sendMidiNoteOn(SENSOR_SWITCH_NOTE, 0, settings.midiChannel);
}
