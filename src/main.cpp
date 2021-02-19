#include <Arduino.h>

#include <config.h>

// ssd1306 oled
#include <Wire.h> // for ssd1306
#include <SPI.h>  // for ssd1306
#include "io/oled.h"

// neopixel leds
#include "io/ws2812.h"
#include "io/indicatorLeds.h"

//Shift In - reads parallel in serial out 74hc165 Shift registers
#include "io/shiftIn.h"
#include <io/button.h>

// lidar
#include <io/lidar.h>

// Analog In - read parallel analog values from the pots
#include "io/analogMux.h"

#include <MIDI.h>
// MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

#include <io/usbmidi.h>
#include <io/midiIO.h>
#include <io/midiRouting.h>

// global vars:
byte uiState = 0; // 0= home, 1 = editing min, 2 = editing max
byte hue = 0;
unsigned long tFrame = 0;

bool redrawOled = false; // flag to trigger oled redraw

void updateMidi() // read and forward usb and din midi
{
  // output midi
  USBMIDI.poll(); // required to call in loop

  // forward midi
  forwardMidiUSBtoUART();
  forwardUARTMidi();
}

void myDelay(int dur) // delay without blocking midi forwarding
{
  long t0 = millis();
  while (millis() - t0 < dur)
  {
    // update here everything that needs updating while delay

    // output midi
    USBMIDI.poll(); // required to call in loop

    // forward midi
    forwardMidiUSBtoUART();
    forwardUARTMidi();
  }
}

// knobs logic
#include <controller.h>

// write to oled
#include <draw.h>
#include <menu.h>

#include <sendMidi.h>
#include <io/animateNeopixel.h>

void setup()
{
  pinMode(PLUG_INDIC_PIN, INPUT); // initialise midi out plug indicator

  if (EEPROM.read(1023) > 3) // initialising memory - should only run the first time
  {
    EEPROM.write(1023, 0); // setting slot 0 as active slot
    
    for (byte i = 0; i < KNOB_AMT; i++) //set channels to default
    {
      knobs[i].midiChannel = 0; // default
      knobs[i].midiCC = i;
      knobs[i].max = 127;
      knobs[i].min = 0;
    }

    for (byte i = 0; i < 4; i++)
    {
      saveConfig(i); // overwrite all saveslots
    }    
  }

  byte slot = EEPROM.read(1023); // read recent save slot number
  loadConfig(slot);
  settings.midiChannel = 1;
  // saveSettings();

  MIDI.begin();

  Serial.begin(31250);

  neopixelBegin();

  for (byte i = 0; i < 4; i++) // turn off leds
  {
    setPixelHsv(i, 0, 0, 0);
  }

  pixels.show();

  shiftInInit();
  analogReference(EXTERNAL); // for hardware prototypes 0.2 and on
  // analogReference(INTERNAL); // for hardware prototypes 0.5 and on, NO EXTERNAL AS1117 3.3V REGULATOR
  muxBegin();

  Wire.begin();
  oledBegin();  // uses wire
  lidarBegin(); // uses wire
  controllerBegin();

  oledPrint("tinyLittlFaderBank", 0, 1, 3);
  delay(300);
  oledPrint("loading slot " + String(slot + 1), 0, 3, 0);
  // delay(400);
  // oledPrint("starting.    ", 0, 3, 0);
  // delay(200);
  // oledPrint("starting..   ", 0, 3, 0);
  delay(600);
  oledPrint("starting...    ", 0, 3, 0);

  for (byte i = 0; i < 48; i++)
  {
    muxRead();
    updateKnobs();
  }

  delay(300);
  oledPrint("SG20", 105, 3, 0);

  for (byte j = 0; j < 255; j++)
  {
    animateNeopixel(j);
    pixels.show();
    delay(2);
  }

  delay(1000);
  oled.clear();
}

void loop()
{
  tFrame = millis();

  // get fader & knobs values
  muxRead();
  // muxDebug(); // get raw values to serial

  // get Button Values
  inputValues = shiftInUpdate();
  updateButtons(tFrame);

  readLidarSensor(8); // set running average sampling amount - slew rate limiter

  // update
  updateKnobs();
  updateMenu();

  updateMidi(); // forwarding Data

  sendButtonMidi();     // midi notes 0-4
  sendControllerMidi(); // send ccs

  // write to Leds
  animateNeopixel();
  midiInIndicator.update();
  midiOutIndicator.update();
  USBIndicator.update();
  pixels.show();

  // write to oled
  if (!menu.active) // menu is drawn by itself, so only draw homescreen when not in menu
    drawHome();

  // Pause before next pass through loop
  // delay(500);
  // delay(5);
}
