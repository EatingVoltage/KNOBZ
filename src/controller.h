#include <Arduino.h>

struct knob_t
{
    // value
    int readBuf = 0;
    int val = 0;
    bool hasNew = false;

    byte min = 0;
    byte max = 127;

    // anim
    // long t0 = 0;
    // int dur = 0;
    // byte origin = 0;
    // byte target = 0;

    // midi
    byte midiChannel = 0;
    byte midiCC = 0;
};

knob_t knobs[KNOB_AMT];

byte activeKnob = 0;

// mappings
// byte faderChannel[12] = {15, 14, 13, 12, 11, 10, 31, 30, 29, 28, 27, 26};
// byte potsChannel[20] = {9, 8, 6, 7, 25, 24, 22, 23, 5, 4, 3, 2, 1, 0, 21, 20, 19, 18, 17, 16};

void updateKnobs()
{
    // get values
    int knobValues[KNOB_AMT]; // all input goes through here
    // for (byte i = 0; i < KNOB_AMT; i++)
    // {
    //     knobValues[i] = 0;
    // }

    // for (byte i = 0; i < 6; i++) // faders
    // {
    //     knobValues[i] = mux_in[faderChannel[i]];
    // }
    // for (byte i = 0; i < 20; i++) // muxed knobs
    // {
    //     knobValues[i + 12] = 1023 - mux_in[potsChannel[i]]; //inverted
    // }

    // multiplexed inputs
    for (byte i = 0; i < 32; i++)
    {
        int x = mux_in[i];
        knobValues[i] = x;
    }

    // for (byte i = 0; i < 32; i++)
    // {
    //     Serial.print(i);
    //     Serial.print(":");
    //     Serial.print(knobValues[i]);
    //     Serial.print("\t");
    // }
    // Serial.println();

    // direct pin inputs
    knobValues[32] = 1023 - analogRead(A2); // inverted
    knobValues[33] = 1023 - analogRead(A3); //
    knobValues[34] = 1023 - analogRead(A4); //
    knobValues[35] = 1023 - analogRead(A5); //

    // air knob - lidar sensor
    int x = constrain(sensorReading, 0, LIDAR_UPPER_LIMIT);
    // knobValues[36] = 0;
    knobValues[36] = map(x, 0, LIDAR_UPPER_LIMIT, 1023, 0);
    // debugLidarSensor();
    // Serial.println(knobValues[36]);

    // for (byte i = 0; i < 37; i++)
    // {
    //     // if (knobValues[i] > 1023)
    //     // {
    //         // Serial.print(i + ": " + (knobValues[i]));
    //         // Serial.println("biiiig: " + String(i) + " " + String(knobValues[i]));
    //         // delay(200);
    //     // }
    // }
    // Serial.println();

    // process readings
    for (byte i = 0; i < KNOB_AMT; i++) // read all knobs
    {
        knobs[i].hasNew = false;

        int reading = knobValues[i];
        if (abs(reading - knobs[i].readBuf) > POT_TRSH) // if a pot was moved
        {
            knobs[i].readBuf = reading;
            int x = map(reading, 0, 1023, 127, 0);
            knobs[i].val = x;
            knobs[i].hasNew = true;
            activeKnob = i;

            // update oled if knob was moved
            if (minButton.pressed)
            {
                knobs[i].min = min(knobs[i].val, 127 - MINMAX_MARGIN);
                redrawOled = true;
                knobs[i].val = knobs[i].min; // to think about. scaling does not match like this
            }
            else if (maxButton.pressed)
            {
                knobs[i].max = max(knobs[i].val, MINMAX_MARGIN);
                redrawOled = true;
                knobs[i].val = knobs[i].max; // to think about. scaling does not match like this
            }

            sendMidiCC(knobs[i].midiCC, knobs[i].val, knobs[i].midiChannel);
        }
    }

    // debugging
    // for (byte i = 0; i < 12; i++)
    // {
    //     if (knobs[i].hasNew)
    //     {
    //         Serial.print("knob ");
    //         Serial.print(i);
    //         Serial.print(" has new ");
    //         Serial.println(knobs[i].val);
    //         // delay(300);
    //     }
    // }
}

// void animateKnob(knob_t &myKnob)
// {
//     byte res = myKnob.target;
//     float perc;
//     if (tFrame < myKnob.t0 + myKnob.dur) // if its animation time
//     {
//         perc = float(tFrame - myKnob.t0) / float(myKnob.dur);
//         res = myKnob.origin + perc * (myKnob.target - myKnob.origin);

//         if (myKnob.val != res)
//         {
//             myKnob.val = res;
//             myKnob.hasNew = true;
//         }
//     }
// }

// void setAnimation(knob_t &myKnob, byte target, int duration)
// {
//     myKnob.t0 = tFrame;
//     myKnob.dur = duration;
//     myKnob.origin = myKnob.val;
//     myKnob.target = target;
// }

void saveConfig(byte slot) // slots 0-3
{
    EEPROM.update(1023, slot);
    for (byte i = 0; i < KNOB_AMT; i++)
    {
        knobConfig_t saveState;
        int addr = sizeof(settings) + (slot * sizeof(knobConfig_t) * KNOB_AMT) + (sizeof(knobConfig_t) * i);
        saveState.midiChannel = knobs[i].midiChannel;
        saveState.cc = knobs[i].midiCC;
        saveState.min = knobs[i].min;
        saveState.max = knobs[i].max;
        // EEPROM.put(addr, saveState);
        EEPROM_writeAnything(addr, saveState);
    }
}

void loadConfig(byte slot)
{
    EEPROM.update(1023, slot); // saving last loaded slot number for startup loading recent preset
    // get data here

    for (byte i = 0; i < KNOB_AMT; i++)
    {
        int addr = sizeof(settings) + (slot * sizeof(knobConfig_t) * KNOB_AMT) + (sizeof(knobConfig_t) * i);
        knobConfig_t saveState; // pack of config values for one knob
        // EEPROM.get(addr, saveState);
        EEPROM_readAnything(addr, saveState);

        knobs[i].midiChannel = saveState.midiChannel;
        knobs[i].midiCC = saveState.cc;
        knobs[i].min = saveState.min;
        knobs[i].max = saveState.max;
    }
    // oledPrint("channel: " + saveState.midiChannel);
}

void controllerBegin() // set midi config
{
    loadConfig(EEPROM.read(1023)); // loading recent preset slot
}