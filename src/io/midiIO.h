#include <Arduino.h>

void sendMidiCC(byte cc, byte val, byte midiChannel)
{
    MIDI.sendControlChange(cc, val, midiChannel + 1);
    usbMidiControlChange(midiChannel, cc, val);
    usbLed.trigger();
    tickMidiOutIndicator();
}

void sendMidiNoteOn(byte pitch, byte velocity, byte midiChannel)
{
    MIDI.sendNoteOn(pitch, velocity, midiChannel + 1);
    usbMidiNoteOn(midiChannel, pitch, velocity);
    usbLed.trigger();
    tickMidiOutIndicator(); // using wrapper function instead of direct method to implement plug indicator logic
}