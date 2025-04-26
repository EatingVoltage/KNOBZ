#include <Arduino.h>

#include <USBMIDI.h>

void usbMidiNoteOn(byte channel, byte pitch, byte velocity)
{
	USBMIDI.write((velocity != 0 ? 0x90 : 0x80) | (channel & 0xf));
	USBMIDI.write(pitch & 0x7f);
	USBMIDI.write(velocity &0x7f);
}

// void usbMidiNoteOff(byte channel, byte pitch, byte velocity)
// {
// 	USBMIDI.write((velocity != 0 ? 0x90 : 0x80) | (channel & 0xf));
// 	USBMIDI.write(pitch & 0x7f);
// 	USBMIDI.write(0 &0x7f);
// }

void usbMidiControlChange(byte channel, byte control, byte value)
{
	USBMIDI.write(0xB0 | (channel & 0xf));
	USBMIDI.write(control & 0x7f);
	USBMIDI.write(value & 0x7f);
}
