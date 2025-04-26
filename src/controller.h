#include <Arduino.h>

class knob_c
{
private:
    int sum = 0; // for running average
    int oldVal = 0;

public:
    byte val = 0;
    bool hasNew = false;

    byte min = 0;
    byte max = 127;

    // midi
    byte midiChannel = 0;
    byte midiCC = 0;

    void update(int reading);
    byte getVal();
};

void knob_c::update(int reading)
{
    // running average
    sum = (sum * (KNOB_AVG_LEN - 1) / KNOB_AVG_LEN) + reading;
    // get reading:
    int x = constrain(map(sum / KNOB_AVG_LEN, 0, 1023, 0, 1023 + 2 * KNOB_DEADZONE) - KNOB_DEADZONE, 0, 1023); // mapping out dead area

    // check if there is new data
    hasNew = false;
    if (abs(x - oldVal) > POT_TRSH)
    {
        oldVal = x;
        val = x / 8; // scaling to final 7 bit
        hasNew = true;
    }
}

byte knob_c::getVal()
{
    return val;
}

knob_c knob[KNOB_AMT];

byte activeKnob = 0;

void updateKnobs()
{
    // feed data to knob objects

    // multiplexed inputs
    for (byte i = 0; i < 30; i++)
    {
        knob[i].update(1023 - mux_in[i]);
    }

    // air knob - lidar sensor
    knob[30].update(map(constrain(sensorReading, 0, LIDAR_UPPER_LIMIT), 0, LIDAR_UPPER_LIMIT, 0, 1023));
    // debugLidarSensor();

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
                    knob[i].val = knob[i].min;
                }
                else if (maxButton.pressed)
                {
                    knob[i].max = max(knob[i].getVal(), MINMAX_MARGIN);
                    redrawOled = true;
                    knob[i].val = knob[i].max;
                }
            }

            else if (!maxButton.pressed && !minButton.pressed)
            {
                activeKnob = i;
            }
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
    EEPROM.update(RECENT_SLOT_EEPROM_ADDR, slot);
    // Serial.println(sizeof(knobConfig_t) * KNOB_AMT * 4);
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
    EEPROM.update(RECENT_SLOT_EEPROM_ADDR, slot); // saving last loaded slot number for startup loading recent preset
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
    loadConfig(EEPROM.read(RECENT_SLOT_EEPROM_ADDR)); // loading recent preset slot
}