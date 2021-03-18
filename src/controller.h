#include <Arduino.h>

// #define KNOB_AVG_LEN 4
// #define KNOB_AVG_LEN 8
#define KNOB_AVG_LEN 16

class knob_c
{
private:
    /* data */
    unsigned int sum = 0; // for running average
    byte readBuf = 0;

public:
    // value

    byte val = 0;
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

    void update(int reading);
    byte getVal();
};

void knob_c::update(int reading)
{
    // running average$
    long x = sum * (KNOB_AVG_LEN - 1);
    x = x / KNOB_AVG_LEN;
    sum = x + reading / 4;

    // check if there is new data

    // Serial.println(analogRead(A4));
    // Serial.println(reading);
    // Serial.println(sum);
    // delay(200);

    val = (sum / KNOB_AVG_LEN);
    val /= 2; // reducing to target 7 bit val

    hasNew = false;
    if (readBuf != val)
    {
        readBuf = val;
        hasNew = true;
    }
}

byte knob_c::getVal()
{
    // int x = (sum / KNOB_AVG_LEN);
    // byte res = x / 2; // reducing to target 7 bit val
    return val;
}

knob_c knob[KNOB_AMT];

byte activeKnob = 0;

void updateKnobs()
{
    // feed data to knob objects

    // multiplexed inputs
    for (byte i = 0; i < 32; i++)
    {
        int x = mux_in[i];
        knob[i].update(1023 - x);
    }

    // direct pin inputs
    knob[32].update(analogRead(A2)); // 
    knob[33].update(analogRead(A3)); //
    knob[34].update(analogRead(A4)); //
    knob[35].update(analogRead(A5)); //

    // // air knob - lidar sensor
    // int x = constrain(sensorReading, 0, LIDAR_UPPER_LIMIT);
    // // knobValues[36] = 0;
    // knobValues[36] = map(x, 0, LIDAR_UPPER_LIMIT, 1023, 0);
    // // debugLidarSensor();

    // process readings
    for (byte i = 0; i < KNOB_AMT; i++) // read all knob
    {
        if (knob[i].hasNew)
        {
            if (i == activeKnob)
            {

                // update oled if knob was moved
                if (minButton.pressed)
                {
                    knob[i].min = min(knob[i].getVal(), 127 - MINMAX_MARGIN);
                    redrawOled = true;
                    knob[i].val = knob[i].min; // to think about. scaling does not match like this
                }
                else if (maxButton.pressed)
                {
                    knob[i].max = max(knob[i].getVal(), MINMAX_MARGIN);
                    redrawOled = true;
                    knob[i].val = knob[i].max; // to think about. scaling does not match like this
                }
            }

            else if(!maxButton.pressed && !minButton.pressed)
                activeKnob = i;

            // sendMidiCC(knob[i].midiCC, knob[i].val, knob[i].midiChannel);
        }
    }

    // debugging
    // for (byte i = 0; i < 12; i++)
    // {
    //     if (knob[i].hasNew)
    //     {
    //         Serial.print("knob ");
    //         Serial.print(i);
    //         Serial.print(" has new ");
    //         Serial.println(knob[i].val);
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
        saveState.midiChannel = knob[i].midiChannel;
        saveState.cc = knob[i].midiCC;
        saveState.min = knob[i].min;
        saveState.max = knob[i].max;
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

        knob[i].midiChannel = saveState.midiChannel;
        knob[i].midiCC = saveState.cc;
        knob[i].min = saveState.min;
        knob[i].max = saveState.max;
    }
    // oledPrint("channel: " + saveState.midiChannel);
}

void controllerBegin() // set midi config
{
    loadConfig(EEPROM.read(1023)); // loading recent preset slot
}