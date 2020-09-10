#include <Arduino.h>

void routeMidiUARTtoUSB()
{
    // MidiType type = MIDI.getType();
    // Serial.println(type);
    //     inline MidiType getType() const;
    // inline Channel  getChannel() const;
    // inline DataByte getData1() const;
    // inline DataByte getData2() const;
    // while (Serial1.available())
    // {
    //     Serial.print(Serial1.read());
    //     tickUSBIndicator();
    // }
}

void routeMidiUSBtoUART()
{
    midiEventPacket_t rx;

    do
    {
        rx = MidiUSB.read();
        if (rx.header != 0)
        {
            //send back the received MIDI command

              Serial1.write(rx.header);
              if(rx.byte1) Serial1.write(rx.byte1);
              if(rx.byte2) Serial1.write(rx.byte2);
              if(rx.byte3) Serial1.write(rx.byte3);
        }
    } while (rx.header != 0);
}
