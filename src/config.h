#include <Arduino.h> // for type definitions

// this file saves and loads the LevelConfig struct to eeprom.

#include <EEPROM.h>
#define EEPROM_ADDR 0 // default eeprom addr for knob state data
#define RECENT_SLOT_EEPROM_ADDR 1022 // only one byte to keep recent saveslot
#define CONTROLLER_CONFIG_EEPROM_ADDR 900 // keeps controller settings

#define PLUG_INDIC_PIN 7 // pin number of plug pin

#define POT_TRSH 8 // knob threshold. must be high enough to get rid of pot/fader noise. 8 is the lowest most setting that can get full 7 bits 

#define KNOB_DEADZONE 8 // this ensures knobs reach extremes. set as low as possible, high enough to reach values 0 and 127

#define KNOB_AVG_LEN 4 // number of samples in running average

#define LED_BRIGHTNESS 50 // ws2812 control var

#define LIDAR_UPPER_LIMIT 400 // ceiling. higher values are omitted

#define LIDAR_SMOOTHING 10 // sample amount

#define MENU_TIMEOUT 5000 // time for menu to disappear

#define INDICATOR_DUR 15 // midi traffic indicator led blink duration ms

#define MIDI_DEFAULT_VEL 64 // velocity for controller button presses

#define KNOB_AMT 31 // number of knob objects

#define MINMAX_MARGIN 10 // limits min to 127-MINMAX_MARGIN and max to MINMAX_MARGIN

struct controllerSettings
{
  byte midiChannel = 1; // channel to send noteOn and noteOffs with controller buttons
};

controllerSettings settings;


// functions to store complex data structures to the eeprom

template <class T>
int EEPROM_writeAnything(int ee, const T &value)
{
    const byte *p = (const byte *)(const void *)&value;
    unsigned int i;

    for (i = 0; i < sizeof(value); i++)
        EEPROM.update(ee++, *p++);

    EEPROM.end();

    return i;
}

template <class T>
int EEPROM_readAnything(int ee, T &value)
{
    byte *p = (byte *)(void *)&value;
    unsigned int i;

    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
}

void saveSettings()
{
    // sava data here
    EEPROM_writeAnything(CONTROLLER_CONFIG_EEPROM_ADDR, settings);
    // EEPROM.put(EEPROM_ADDR, settings);
}

controllerSettings loadSettings()
{
    controllerSettings res;
    // load data here
    EEPROM_readAnything(CONTROLLER_CONFIG_EEPROM_ADDR, res);
    // EEPROM.get(EEPROM_ADDR, settings);
    return res;
}

// saving knob settings
struct knobConfig_t
{
    byte midiChannel;
    byte cc;
    byte min;
    byte max;
};
