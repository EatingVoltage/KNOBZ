#include <Arduino.h>
/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

// #include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void lidarBegin()
{
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    oled.clear();
    oledPrint(F("lidar init error."), 0, 0, 1);
    while (1)
    {
    }
  }
  sensor.startContinuous();
}

long sensorReading;
long oldSensorReading;

void readLidarSensor(byte sampleAmt)
{
  int reading = sensor.readRangeContinuousMillimeters() - 70;
  if (reading < 1000 && reading)
    sensorReading = (sensorReading / sampleAmt) * (sampleAmt - 1) + reading / sampleAmt; // running average
  if (sensor.timeoutOccurred())
  {
    oled.clear();
    oledPrint("LIDAR TIMEOUT ERROR", 0, 0, 1);
    delay(500);
  } // error checking
}

// void debugLidarSensor()
// {
//     String res = "";
//     for (byte i = 0; i < sensorReading/10; i++)
//     {
//       res += "#";
//     }
//     Serial.println(res);
//     Serial.println(sensorReading);
// }
