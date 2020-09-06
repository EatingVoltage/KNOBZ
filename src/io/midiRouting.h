#include <Arduino.h>

// struct midiMessage =
// {
//     MIDI.MidiType type = MIDI.getType();
// }

void routeMidiUARTtoUSB()
{
    // MidiType type = MIDI.getType();
    // Serial.println(type);
    //     inline MidiType getType() const;
    // inline Channel  getChannel() const;
    // inline DataByte getData1() const;
    // inline DataByte getData2() const;
    while (Serial1.available())
    {
        Serial.print(Serial1.read());
        setPixel(2, 0, 0, 255);
        pixels.show();
    }
}

void routeMidiUSBtoUART()
{
    // while (Serial.available())
    // {
    //     Serial1.print(Serial.read());
    //     setPixel(0, 0, 0, 255);
    //     pixels.show();
    // }
    midiEventPacket_t rx;

    do
    {
        rx = MidiUSB.read();
        if (rx.header != 0)
        {
            Serial.print(rx.header);
            Serial.print(" ");
            Serial.print(rx.byte1);
            Serial.print(" ");
            Serial.print(rx.byte2);
            Serial.print(" ");
            Serial.print(rx.byte3);
            Serial.println(" ");  
            //send back the received MIDI command

            //   Serial.write();
        }
    } while (rx.header != 0);
}
