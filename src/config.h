#include <Arduino.h> // for type definitions

// this file saves and loads the LevelConfig struct to eeprom.

#include <EEPROM.h>

#define POT_TRSH 15

#define LED_BRIGHTNESS 20

#define LIDAR_UPPER_LIMIT 400

#define LIDAR_SMOOTHING 10

#define MENU_TIMEOUT 5000

#define INDICATOR_DUR 15 // midi traffic indicator led blink duration ms

#define MIDI_DEFAULT_VEL 64

struct controllerSettings
{
  byte midiChannel = 1; // channel to send noteOn and noteOffs with controller buttons
};

controllerSettings settings;

#define EEPROM_ADDR 0

// functions to store complex data structures to the eeprom

template <class T>
int EEPROM_writeAnything(int ee, const T &value)
{
    const byte *p = (const byte *)(const void *)&value;
    unsigned int i;

    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);

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
    EEPROM_writeAnything(EEPROM_ADDR, settings);
    // EEPROM.put(EEPROM_ADDR, settings);
}

controllerSettings loadSettings()
{
    controllerSettings res;
    // load data here
    EEPROM_readAnything(EEPROM_ADDR, res);
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
