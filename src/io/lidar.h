#include <Arduino.h>
/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

// #include <Wire.h>
#include <VL53L0X.h>

VL53L0X lidar;

void lidarBegin()
{
  // Wire.begin();
  lidar.setTimeout(500);
  lidar.init();
  lidar.startContinuous();
}

long sensorReading;

void readLidarSensor(byte sampleAmt)
{
  if (!shiftInReadBit(7)) // if toggle switch is set to on
  {
    int reading = lidar.readRangeContinuousMillimeters() - 70;
    if (reading < 1000 && reading)
      sensorReading = reading; //
      // sensorReading = (sensorReading / sampleAmt) * (sampleAmt - 1) + reading / sampleAmt; // running average
    // if (lidar.timeoutOccurred())
    // {
    // oledPrint("LIDAR TIMEOUT ERROR", 0, 0, 1);
    // delay(500);
    // } // error checking
  }
}
