#include <Arduino.h>

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