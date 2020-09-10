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
    long t0 = 0;
    // int dur = 0;
    byte origin = 0;
    byte target = 0;

    // midi
    byte midiChannel = 0;
    byte midiCC = 0;
};

knob_t knobs[KNOB_AMT];

void controllerBegin() // set midi config
{
    for (byte i = 0; i < KNOB_AMT; i++)
    {
        knobs[i].midiChannel = settings.midiChannel;
        knobs[i].midiCC = i;
    }
    // Serial.println(sizeof(knobs));
    // read from eeprom here
}

byte activeKnob = 0;

// mappings
byte faderChannel[12] = {15, 14, 13, 12, 11, 10, 31, 30, 29, 28, 27, 26};
byte potsChannel[20] = {9, 8, 6, 7, 25, 24, 22, 23, 5, 4, 3, 2, 1, 0, 21, 20, 19, 18, 17, 16};

void updateKnobs()
{
    // get values
    int knobValues[KNOB_AMT];           // all input goes through here
    for (byte i = 0; i < 12; i++) // faders
    {
        knobValues[i] = mux_in[faderChannel[i]];
    }
    for (byte i = 0; i < 20; i++) // muxed knobs
    {
        knobValues[i + 12] = 1023 - mux_in[potsChannel[i]]; //inverted
    }
    // direct knobs
    knobValues[32] = 1023 - analogRead(A2); // inverted
    knobValues[33] = 1023 - analogRead(A3); //
    knobValues[34] = 1023 - analogRead(A4); //
    knobValues[35] = 1023 - analogRead(A5); //

    // air knob - lidar sensor

    int x = constrain(sensorReading, 0, LIDAR_UPPER_LIMIT);
    // Serial.println(knobValues[36]);
    knobValues[36] = map(x, 0, LIDAR_UPPER_LIMIT, 1023, 0);
    // debugLidarSensor();
    // Serial.println(knobValues[36]);

    // process readings
    for (byte i = 0; i < KNOB_AMT; i++) // read all knobs
    {
        knobs[i].hasNew = false;

        int reading = knobValues[i];
        if (abs(reading - knobs[i].readBuf) > POT_TRSH) // if a pot was moved
        {
            knobs[i].readBuf = reading;
            knobs[i].val = map(knobs[i].readBuf, 0, 1023, 127, 0);
            knobs[i].hasNew = true;
            sendMidiCC(knobs[i].midiCC, knobs[i].val, knobs[i].midiChannel);
            activeKnob = i;
            if (minButton.pressed)
            {
                knobs[i].min = min(knobs[i].val, 127-MINMAX_MARGIN);
                redrawOled = true;
            }
            else if (maxButton.pressed)
            {
                knobs[i].max = max(knobs[i].val, MINMAX_MARGIN);
                redrawOled = true;
            }

            // if (tFrame < knobs[i].t0 + knobs[i].dur) // if animating
            // {
            //     knobs[i].origin = knobs[i].val;
            // }
        }
    }

    // debugging
    // if (knobs[36].hasNew)
    // Serial.println(knobs[36].val);
    // for (byte i = 0; i < 12; i++)
    // {
    //     if (faders[i].hasNew)
    //     {
    //         Serial.print("fader ");
    //         Serial.print(i);
    //         Serial.print(" has new ");
    //         Serial.println(faders[i].val);
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
    EEPROM.update(1023, slot);
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