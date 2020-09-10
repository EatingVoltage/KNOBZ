#include <Arduino.h>

#include <config.h>

// ssd1306 oled
#include <Wire.h> // for ssd1306
#include <SPI.h>  // for ssd1306
#include "io/oled.h"

// neopixel leds
#include "io/ws2812.h"
#include <io/indicatorLeds.h> 

// lidar
#include <io/lidar.h>

//Shift In - reads parallel in serial out 74hc165 Shift registers
#include "io/shiftIn.h"
#include <io/button.h>

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

// knobs logic
#include <controller.h>

// write to oled
#include <draw.h>
#include <menu.h>

#include <sendMidi.h>
#include <io/animateNeopixel.h>


void setup()
{
  // MidiUSB.flush();

  settings = loadSettings();
  // settings.midiChannel = 1;
  // settings.midiDefaultVel = 63;
  // saveSettings();

  MIDI.begin();

  Serial.begin(31250);
  // while (!Serial)
  // {
  // delay(100);
  // }
  // Serial.println("starting");

  neopixelBegin();

  for (byte i = 0; i < 4; i++) // turn off leds
  {
    setPixel(i, 0, 0, 0);
  }

  pixels.show();

  shiftInInit();
  // analogReference(INTERNAL); // for hardware prototypes 0.2 and on
  muxBegin();

  Wire.begin();
  oledBegin();
  lidarBegin(); // needs wire
  controllerBegin();

  // delay(200);
  oledPrint("Fader Knobbins", 10, 1, 3);
  delay(500);
  oledPrint("starting...", 0, 3, 0);
  delay(200);
  oledPrint("SG20", 100, 3, 0);

  // TEST ZONE
  // oled.clear();
  // oledPrint(String(sizeof()), 0, 0, 0);

  //

  byte val = 0;
  for (byte i = 0; i < 12; i++)
  {
    // read from knobs a few times
    muxRead();
    updateKnobs();
  }
  byte h = getHueFromKnobs();

  for (byte j = 0; j < 255; j++)
  {
    animateNeopixel(j);
    pixels.show();
    delay(2);
  }

  delay(1000);
  MidiUSB.flush();
  oled.clear();
}

void loop()
{
  tFrame = millis();

  // oledPrint(String(tFrame), 0, 0, 5);
  // Serial.println("ms: "+ String(tFrame));

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

  // output midi
  sendButtonMidi();     // midi notes 0-4
  sendControllerMidi(); // send
  forwardMidi();

  // write to Leds
  animateNeopixel();
  updateIndicators();
  pixels.show();

  // write to oled
  if (!menu.active) // menu is drawn by itself, so only draw homescreen when not in menu
    drawHome();

  MidiUSB.flush(); // send usb midi - clear buffer needed for stable operation

  // Pause before next pass through loop
  // delay(50);
  delay(5);
}
