#include <Arduino.h>

// activate midi / Serial debug report of midi send events

// const bool sendSerialDebug = true;
// const bool sendMidi = false;

const bool sendSerialDebug = false;
const bool sendMidi = true;

void sendMidiCC(byte cc, byte val, byte midiChannel)
{
    setPixel(1, 0, 0, 255); // set indicator leds
    setPixel(2, 0, 0, 255);
    pixels.show();
    // delay(10);

    MIDI.sendControlChange(cc, val, midiChannel);
    if (sendMidi)
    {
        usbMidiControlChange(midiChannel, cc, val);
        MidiUSB.flush();
    }
    if (sendSerialDebug)
        Serial.println("CC" + String(cc) + ": " + String(val));
}

void sendMidiPitchbend(int val, byte midiChannel)
{
    if (sendMidi)
        MIDI.sendPitchBend(val, midiChannel);
    if (sendSerialDebug)
        Serial.println("Pitchbend: " + String(val));
}

void sendMidiNoteOn(byte pitch, byte velocity, byte midiChannel)
{
    MIDI.sendNoteOn(pitch, velocity, midiChannel);
    if (sendMidi)
    {
        usbMidiNoteOn(midiChannel, pitch, velocity);
    }
    if (sendSerialDebug)
        Serial.println("Note on " + String(pitch));
}

void sendMidiNoteOff(byte pitch, byte midiChannel)
{
    MIDI.sendNoteOff(pitch, 0, midiChannel);
    if (sendMidi)
    {
        usbMidiNoteOff(midiChannel, pitch, 0);
    }
    if (sendSerialDebug)
        Serial.println("Note off " + String(pitch));
}

void allNotesOff(byte midiChannel)
{
    for (byte i = 0; i > 128; i++)
    {
        sendMidiNoteOff(i, midiChannel);
        if (sendMidi)
            usbMidiNoteOff(midiChannel, i, 0);
    }
}
