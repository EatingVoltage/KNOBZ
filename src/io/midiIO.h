#include <Arduino.h>

#define sendMidi 1 // activates usb midi sending

void sendMidiCC(byte cc, byte val, byte midiChannel)
{
    tickMidiOutIndicator(millis());
    MIDI.sendControlChange(cc, val, midiChannel + 1);
    // MIDI.sendControlChange(cc, val, midiChannel);
    if (sendMidi)
    {
        usbLed.trigger(millis());
        usbMidiControlChange(midiChannel + 1, cc, val);
        // MidiUSB.flush();
        // if (sendSerialDebug)
        //     Serial.println("CC" + String(cc) + ": " + String(val) + " CH:" + String(midiChannel));
    }

    // void sendMidiPitchbend(int val, byte midiChannel)
    // {
    //     if (sendMidi)
    //         MIDI.sendPitchBend(val, midiChannel+1);
    //     // if (sendSerialDebug)
    //     //     Serial.println("Pitchbend: " + String(val));
}

void sendMidiNoteOn(byte pitch, byte velocity, byte midiChannel)
{
    tickMidiOutIndicator(millis()); // using wrapper function instead of direct method to implement plug indicator logic
    MIDI.sendNoteOn(pitch, velocity, midiChannel + 1);
    if (sendMidi)
    {
        usbMidiNoteOn(midiChannel + 1, pitch, velocity);
    }
    // if (sendSerialDebug)
    //     Serial.println("Note on " + String(pitch));
}

// void sendMidiNoteOff(byte pitch, byte midiChannel)
// {
//     MIDI.sendNoteOff(pitch, 0, midiChannel+1);
//     if (sendMidi)
//     {
//         usbMidiNoteOff(midiChannel+1, pitch, 0);
//     }
//     // if (sendSerialDebug)
//     //     Serial.println("Note off " + String(pitch));
// }
