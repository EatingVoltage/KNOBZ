#include <Arduino.h>

void forwardMidiUSBtoUART()
{
  while (USBMIDI.available())
  {
    Serial1.write(USBMIDI.read());
    outLed.trigger();
    usbLed.trigger();
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
    inLed.trigger();
    usbLed.trigger();

  }
  USBMIDI.flush();
}