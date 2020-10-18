#include <Arduino.h>

// // usb midi
// #include "MIDIUSB.h"

// // First parameter is the event type (0x0B = control change).
// // Second parameter is the event type, combined with the channel.
// // Third parameter is the control number number (0-119).
// // Fourth parameter is the control value (0-127).

// void usbMidiNoteOn(byte channel, byte pitch, byte velocity)
// {
//   midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
//   MidiUSB.sendMIDI(noteOn);
// }

// void usbMidiNoteOff(byte channel, byte pitch, byte velocity)
// {
//   midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
//   MidiUSB.sendMIDI(noteOff);
// }

// void usbMidiControlChange(byte channel, byte control, byte value)
// {
//   midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
//   MidiUSB.sendMIDI(event);
// }

#include <USBMIDI.h>

void usbMidiNoteOn(byte channel, byte pitch, byte velocity)
{
	USBMIDI.write((velocity != 0 ? 0x90 : 0x80) | (channel & 0xf));
	USBMIDI.write(pitch & 0x7f);
	USBMIDI.write(velocity &0x7f);
}

void usbMidiNoteOff(byte channel, byte pitch, byte velocity)
{
	USBMIDI.write((velocity != 0 ? 0x90 : 0x80) | (channel & 0xf));
	USBMIDI.write(pitch & 0x7f);
	USBMIDI.write(0 &0x7f);
}

void usbMidiControlChange(byte channel, byte control, byte value)
{
	USBMIDI.write(0xB0 | (channel & 0xf));
	USBMIDI.write(control & 0x7f);
	USBMIDI.write(value & 0x7f);
}
