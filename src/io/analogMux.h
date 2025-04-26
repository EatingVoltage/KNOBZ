#include <Arduino.h>

// read analog data from 74hc4067 analog multiplexers

#define MUX_CHANNELS_AMT 30 

// analog in pin definitions
const int MUX_S[4] = {8, 9, 10, 11};
#define MUX_0_SIG A0
#define MUX_1_SIG A1

int mux_in[30]; // readings

void muxBegin()
{
    for (byte i = 0; i < 4; i++)
    {
        pinMode(MUX_S[i], OUTPUT);
        digitalWrite(MUX_S[i], LOW);
    }
}

void muxRead()
{
    for (int pos = 0; pos < 15; pos++)
    {
        // set mux pins
        if (pos % 2)
            digitalWrite(MUX_S[0], HIGH);
        else
            digitalWrite(MUX_S[0], LOW);

        if ((pos / 2) % 2)
            digitalWrite(MUX_S[1], HIGH);
        else
            digitalWrite(MUX_S[1], LOW);

        if ((pos / 4) % 2)
            digitalWrite(MUX_S[2], HIGH);
        else
            digitalWrite(MUX_S[2], LOW);

        if ((pos / 8) % 2)
            digitalWrite(MUX_S[3], HIGH);
        else
            digitalWrite(MUX_S[3], LOW);

        delayMicroseconds(10);
        // do 2 readings
        mux_in[pos] = analogRead(MUX_0_SIG);
        mux_in[pos + 15] = analogRead(MUX_1_SIG);
    }
}

void muxDebug()
{
    for (byte i = 0; i < MUX_CHANNELS_AMT; i++)
    {
        Serial.print(i);
        Serial.print(":");
        Serial.print(mux_in[i]);
        Serial.print("\t");
    }
    Serial.println();
    delay(300);
}