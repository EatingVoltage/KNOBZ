#include <Arduino.h>

//Shift In - reads parallel in serial out 74hc165 Shift registers

int ploadPin = 12; // Load Input
int dataPin = 5;   // Serial Input
int clockPin = 6;  // CLOCK Input

#define DATA_WIDTH 8 // one chip, only 7 buttons connected
unsigned long inputValues, oldInputValues;

bool shiftInReadBit(byte index) // get bool from inputValues
{
  bool res;
  if (index < DATA_WIDTH)
  {
    res = inputValues >> index & 1;
    return res;
  }
  else
    Serial.println("invalid index");
}

unsigned long shiftInUpdate() // read all data from shift registers
{
  long bitVal;
  unsigned long bytesVal = 0;
  // digitalWrite(PIN_CE, HIGH);
  digitalWrite(ploadPin, LOW);
  delayMicroseconds(5);
  digitalWrite(ploadPin, HIGH);

  for (byte i = 0; i < DATA_WIDTH; i++)
  {
    bitVal = digitalRead(dataPin);
    bytesVal |= (bitVal << ((DATA_WIDTH - 1) - i));
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(clockPin, LOW);
  }
  return (bytesVal);
}

// todo: add reader that only reads one value per frame to optimize performance

void shiftInInit()
{
  pinMode(ploadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(clockPin, LOW);
  digitalWrite(ploadPin, HIGH);
  inputValues = shiftInUpdate();
  oldInputValues = inputValues;
}

void shiftInDebug()
{
  // printByte();
  for (byte i = 0; i < DATA_WIDTH; i++)
  {
    // Serial.print(buttonReading[i]);
    Serial.print(shiftInReadBit(i));
    Serial.print(" ");
  }
  Serial.println();
}