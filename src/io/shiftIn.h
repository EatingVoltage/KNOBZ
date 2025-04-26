#include <Arduino.h>

//Shift In - reads parallel in serial out 74hc165 Shift registers

#define P_LOAD_PIN 12 // Load output
#define DATA_PIN 5    // Serial Input
#define CLOCK_PIN 6   // CLOCK output

#define DATA_WIDTH 8 // one chip, only 7 buttons connected
unsigned long inputValues, oldInputValues;

bool shiftInReadBit(byte index) // get bool from inputValues
{
  return inputValues >> index & 1;
}

unsigned long shiftInUpdate() // read all data from shift registers
{
  long bitVal;
  unsigned long bytesVal = 0;
  // digitalWrite(PIN_CE, HIGH);
  digitalWrite(P_LOAD_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(P_LOAD_PIN, HIGH);

  for (byte i = 0; i < DATA_WIDTH; i++)
  {
    bitVal = digitalRead(DATA_PIN);
    bytesVal |= (bitVal << ((DATA_WIDTH - 1) - i));
    digitalWrite(CLOCK_PIN, HIGH);
    delayMicroseconds(2);
    digitalWrite(CLOCK_PIN, LOW);
  }
  return (bytesVal);
}

// todo: add reader that only reads one value per frame to optimize performance

void shiftInInit()
{
  pinMode(P_LOAD_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(P_LOAD_PIN, HIGH);
  inputValues = shiftInUpdate();
  oldInputValues = inputValues;
}

// void shiftInDebug()
// {
//   // printByte();
//   for (byte i = 0; i < DATA_WIDTH; i++)
//   {
//     // Serial.print(buttonReading[i]);
//     Serial.print(shiftInReadBit(i));
//     Serial.print(" ");
//   }
//   Serial.println();
// }