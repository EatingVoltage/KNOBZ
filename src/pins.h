#pragma once
#include <Arduino.h>

// Analog multiplexer
#define MUX_0_SIG A0
#define MUX_1_SIG A1
const int MUX_S[4] = {8, 9, 10, 11};

// Shift register (74HC165)
#define P_LOAD_PIN 12
#define DATA_PIN   5
#define CLOCK_PIN  6

// NeoPixel LED ring
#define LED_PIN 13

// Plug detect indicator
#define PLUG_INDIC_PIN 7
