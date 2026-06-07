#include <Arduino.h>

// Need to copy files from pio/libdep/ssd1306Ascii manually

// Simple I2C test for 128x32 oled.
// Use smaller faster AvrI2c class in place of Wire.
// Edit AVRI2C_FASTMODE in SSD1306Ascii.h to change the default I2C frequency.
//
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define SSD1306_I2C_ADDRESS 0x3C
// #define SSD1306_I2C_ADDRESS 0x3D

SSD1306AsciiAvrI2c oled;

void oledBegin()
{
    // TODO: try define guard for wire.begin()
    oled.begin(&Adafruit128x32, SSD1306_I2C_ADDRESS);
}

void oledAt(byte col = 0, byte row = 0, byte style = 0) // set font + cursor
{
    switch (style)
    {
    case 0:
        oled.setFont(font5x7);
        break;

    case 1:
        oled.setFont(ZevvPeep8x16);
        break;

// TODO case 2

    case 3:
        oled.setFont(Arial_bold_14);
        break;

    default:
        break;
    }
    oled.set1X();
    oled.setCursor(col, row);
}

// flash-string print. avoids Arduino String class (saves flash)
void oledPrint(const __FlashStringHelper *text, byte col = 0, byte row = 0, byte style = 0)
{
    oledAt(col, row, style);
    oled.print(text);
}

void oledPrintPad3(byte v) // right-align number to width 3 with leading spaces
{
    if (v < 100)
        oled.print(' ');
    if (v < 10)
        oled.print(' ');
    oled.print(v);
}