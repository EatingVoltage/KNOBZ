#include <Arduino.h>

void forwardMidiUSBtoUART()
{
  while (USBMIDI.available())
  {
    Serial1.write(USBMIDI.read());
    // midiOutIndicator.tick();
    outLed.trigger(millis());
    // USBIndicator.tick();
    usbLed.trigger(millis());
  }
  Serial1.flush();
}

void forwardUARTMidi(long tFrame)
{
  while (Serial1.available())
  {
    byte msg = Serial1.read();
    USBMIDI.write(msg);
    Serial1.write(msg);
    inLed.trigger(tFrame);
    usbLed.trigger(tFrame);

  }
  USBMIDI.flush();
}