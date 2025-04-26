#include <Arduino.h>

#include <config.h>

// // ssd1306 oled
#include <Wire.h> // for ssd1306
#include <SPI.h>  // for ssd1306
#include "io/oled.h"

// neopixel leds
#include "io/ws2812.h"
#include "io/Ledling.h"

//Shift In - reads parallel in serial out 74hc165 Shift registers
#include "io/shiftIn.h"
#include <io/button.h>

// lidar
#include <io/lidar.h>

// // Analog In - read parallel analog values from the pots
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

bool redrawOled = false; // flag to trigger oled redraw

void updateMidi() // read and forward usb and din midi
{
  // output midi
  USBMIDI.poll(); // required to call in loop

  // forward midi
  forwardMidiUSBtoUART();
  forwardUARTMidi();
}

void myDelay(unsigned long dur) // delay without blocking midi forwarding
{
  long t0 = millis();
  while (millis() - t0 < dur)
  {
    updateMidi();
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

  byte slot = EEPROM.read(RECENT_SLOT_EEPROM_ADDR); // read recent save slot number

  if (slot > 3) // initialising memory - should only run the first time the device boots
  {
    // commented out for memory saving. lets see if it works.
    // oledPrint("Welcome");
    // slot = 0;
    // EEPROM.write(1023, 0); // setting slot 0 as active slot

    // for (byte i = 0; i < KNOB_AMT; i++) //set channels to default
    // {
    //   knob[i].midiChannel = 0; // default
    //   knob[i].midiCC = i;
    //   knob[i].max = 127;
    //   knob[i].min = 0;
    // }

    // for (byte i = 0; i < 4; i++)
    // {
    //   saveConfig(i); // overwrite all saveslots
    // }

    // settings.midiChannel = 0;
    // saveSettings();
  }

  else // regular startup
  {
    loadConfig(slot);
    settings = loadSettings();
  }

  MIDI.begin();

  neopixelBegin();

  for (byte i = 0; i < 4; i++) // turn off leds
  {
    setPixelHsv(i, 0, 0, 0);
  }
  pixels.show();

  // init hardware input
  shiftInInit();
  analogReference(EXTERNAL); // using 3.3v linear regulator
  muxBegin();

  Wire.begin();
  Wire.setClock(400000);
  oledBegin();  // uses wire
  lidarBegin(); // uses wire

  // init engine
  controllerBegin();

  // startup anim
  String s = "KNOBZ";
  oled.setCursor(42, 0);
  oled.setFont(Arial_bold_14);
  for (byte i = 0; i < s.length(); i++)
  {
    oled.print(s[i]);
    delay(60);
  }
  delay(600);
  oledPrint("- by eatingVoltage -", 5, 2, 0);
  oledPrint("FW b0.1 4/25", 30, 3, 0);
  delay(800);
  oled.setCursor(0, 3);
  oled.clearToEOL();
  oledPrint("Loading Slot " + String(slot + 1), 0, 3, 0);
  delay(800);
  // oled.clearToEOL();
  oledPrint(" ok.", 105, 3, 0);

  // oled.setCol(0);
  // oled.clearToEOL();

  oled.setCursor(0, 3);
  for (byte j = 0; j < 255; j++)
  {
    muxRead();
    updateKnobs();
    for (byte i = 0; i < KNOB_AMT; i++)
    {
      knob[i].hasNew = false;
    }
    updateLedlings();
    animateNeopixel(j);
    pixels.show();
    if (j % (255/24) == 1)
      oled.print(".");
    delay(1);
  }

  // hold menu button to inhibit starting. upload new firmware this way
  while (modeButton.pressed)
  {
    updateButtons();
    delay(200);
  }

  redrawOled = true;
  drawHome();
}

void loop()
{
  // get fader & knobs values
  muxRead();
  // muxDebug(); // get raw values to serial

  // get Button Values
  inputValues = shiftInUpdate();
  updateButtons();

  readLidarSensor(8); // set running average sampling amount - slew rate limiter. 8 works well

  // hold menu button to send all knobs
  if (modeButton.held) // hacky way to send all knob states. manipulates data at mux_in
  {
    for (byte i = 0; i < MUX_CHANNELS_AMT; i++)
    {
      if (mux_in[i] < 511)
        mux_in[i] += 50;
      else
        mux_in[i] -= 50;
    }
  }

  // update
  updateKnobs();
  updateMenu();

  updateMidi();         // forwarding Data
  sendButtonMidi();     // midi notes 0-4
  sendControllerMidi(); // send ccs

  // write to Leds
  updateLedlings();
  animateNeopixel();
  pixels.show();

  // write to oled
  if (!menu.active) // menu is drawn by itself, so only draw homescreen when not in menu
    drawHome();
}


// performance testing

// #define SAMPLE_AMT 32

// int values[SAMPLE_AMT];

// byte counter = 0;

// void setup()
// {
//   analogReference(EXTERNAL); // for hardware prototypes 0.2 and on
//   Serial.begin(31250);
//   // while (!Serial)
//   // {
//   //   delay(10);
//   // }
//     delay(1000);
//   for (byte i = 0; i < SAMPLE_AMT; i++)
//   {
//     values[i] = 0;
//   }
//   for (byte i = 0; i < 4; i++)
//   {
//     pinMode(MUX_S[i], OUTPUT);
//     digitalWrite(MUX_S[i], LOW);
//   }
//   Serial.println("start");

//     Wire.begin();
//   oledBegin();  // uses wire
// }

// void loop()
// {
//   int reading = analogRead(A0);
//   values[counter] = reading;

//   if (counter == SAMPLE_AMT)
//   {
//     counter = 0;

//     int minVal = values[0];
//     for (byte i = 0; i < SAMPLE_AMT - 1; i++)
//     {
//       minVal = min(values[i + 1], minVal);
//     }
//     int maxVal = values[0];
//     for (byte i = 0; i < SAMPLE_AMT - 1; i++)
//     {
//       maxVal = max(values[i + 1], maxVal);
//     }
//     Serial.print("min: " + String(minVal) + " - max: " + String(maxVal));
//     Serial.println(" - diff: " + String(maxVal - minVal));
//     oledPrint(String(minVal) + "   ", 0, 1, 0);
//     oledPrint(String(maxVal) + "   ", 64, 1, 0);
//     oledPrint(String(maxVal-minVal) + "   ", 32, 3, 0);
//     delay(500);
//   }

//   else
//   {
//     counter++;
//   }

//   delay(1);
// }