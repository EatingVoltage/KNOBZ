#include <Arduino.h> // for type definitions

// this file saves and loads the LevelConfig struct to eeprom.

#include <EEPROM.h>

const byte POT_TRSH = 15;

const byte LED_BRIGHTNESS = 20;

const int LIDAR_UPPER_LIMIT = 400;

const int MENU_TIMEOUT = 5000;

struct controllerSettings
{
  byte midiChannel = 1;
  byte midiDefaultVel = 20;
};

controllerSettings settings;

const byte EEPROM_ADDR = 0;

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
}

controllerSettings loadSettings()
{
    controllerSettings res;
    // load data here
    EEPROM_readAnything(EEPROM_ADDR, res);
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

struct saveState_t
{
    knobConfig_t knobConfig[37];
};
