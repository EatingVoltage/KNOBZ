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

SSD1306AsciiAvrI2c oled;

void oledBegin()
{
    oled.begin(&Adafruit128x32, SSD1306_I2C_ADDRESS);
}

void oledPrint(String text, byte col=0, byte row=0, byte style=0)
{
    switch (style)
    {
    case 0:
        oled.setFont(font5x7);
        oled.set1X();
        break;

    case 1:
        oled.setFont(ZevvPeep8x16);
        oled.set1X();
        break;

    case 3:
        oled.setFont(Arial_bold_14);
        oled.set1X();
        break;

    default:
        break;
    }
    oled.setCursor(col, row);
    oled.print(text);
}