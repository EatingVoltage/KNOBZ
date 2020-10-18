#include <Arduino.h>

void routeMidiUARTtoUSB()
{

    // midiEventPacket_t msg;
    // msg.byte1 = MIDI.getData1();
    // msg.byte2 = MIDI.getData2();
    // msg.byte3 = 0;
    // msg.header = MIDI.getStatus(MIDI.getType(), MIDI.getChannel());

    // MidiUSB.sendMIDI(msg);
    // MidiUSB.flush();

    // USBIndicator.tick();
}

void forwardMidiUSBtoUART()
{
  while (USBMIDI.available())
  {
    Serial1.write(USBMIDI.read());
    midiOutIndicator.tick();
    USBIndicator.tick();
  }
  Serial1.flush();
}

void forwardUARTMidi()
{
  while (Serial1.available())
  {
    byte msg = Serial1.read();
    USBMIDI.write(msg);
    Serial1.write(msg);
    midiInIndicator.tick();
    USBIndicator.tick();

  }
  USBMIDI.flush();
}