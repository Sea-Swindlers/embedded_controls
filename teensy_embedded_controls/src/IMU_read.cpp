#include <IMU_read.h>

#include <vector>
#include <string>
#include <functional>

#include <stdio.h>

#include <arduino.h>

#include "ICM_20948_lib/ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU


#define SERIAL_PORT Serial

#define WIRE_PORT Wire // Your desired Wire port.
#define AD0_VAL 1      // The value of the last bit of the I2C address.                \
                       // On the SparkFun 9DoF IMU breakout the default is 1, and when \
                       // the ADR jumper is closed the value becomes 0

ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object


// Below here are some helper functions to print the data nicely!

void printPaddedInt16b(int16_t val)
{
  if (val > 0)
  {
    SERIAL_PORT.print(" ");
    if (val < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  else
  {
    SERIAL_PORT.print("-");
    if (abs(val) < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  SERIAL_PORT.print(abs(val));
}

void printRawAGMT(ICM_20948_AGMT_t agmt)
{
  SERIAL_PORT.print("RAW. Acc [ ");
  printPaddedInt16b(agmt.acc.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.acc.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.acc.axes.z);
  SERIAL_PORT.print(" ], Gyr [ ");
  printPaddedInt16b(agmt.gyr.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.gyr.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.gyr.axes.z);
  SERIAL_PORT.print(" ], Mag [ ");
  printPaddedInt16b(agmt.mag.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.mag.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.mag.axes.z);
  SERIAL_PORT.print(" ], Tmp [ ");
  printPaddedInt16b(agmt.tmp.val);
  SERIAL_PORT.print(" ]");
  SERIAL_PORT.println();
}

std::string printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
  std::string result;
  float aval = abs(val);
  if (val < 0)
  {
    result += "-";
  }
  else
  {
    result += " ";
  }
  for (uint8_t indi = 0; indi < leading; indi++)
  {
    uint32_t tenpow = 0;
    if (indi < (leading - 1))
    {
      tenpow = 1;
    }
    for (uint8_t c = 0; c < (leading - 1 - indi); c++)
    {
      tenpow *= 10;
    }
    if (aval < tenpow)
    {
      result += "0";
    }
    else
    {
      break;
    }
  }
  if (val < 0)
  {
    result += -val;
  }
  else
  {
    result += val;
  }
  
  return result;
}


std::string printScaledAGMT(ICM_20948_I2C *sensor)
{
  std::string result;
  result += "Scaled. Acc (mg) [ ";
  result += printFormattedFloat(sensor->accX(), 5, 2);
  result += ", ";
  result += printFormattedFloat(sensor->accY(), 5, 2);
  result += ", ";
  result += printFormattedFloat(sensor->accZ(), 5, 2);
  result += " ], Gyr (DPS) [ ";
  result += printFormattedFloat(sensor->gyrX(), 5, 2);
  result += ", ";
  result += printFormattedFloat(sensor->gyrY(), 5, 2);
  result += ", ";
  result += printFormattedFloat(sensor->gyrZ(), 5, 2);
  result += " ], Mag (uT) [ ";
  result += printFormattedFloat(sensor->magX(), 5, 2);
  result += ", ";
  result += printFormattedFloat(sensor->magY(), 5, 2);
  result += ", ";
  result += printFormattedFloat(sensor->magZ(), 5, 2);
  result += " ], Tmp (C) [ ";
  result += printFormattedFloat(sensor->temp(), 5, 2);
  result += " ]\n";
  

  return result;
}

void setup_imu()
{

  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);

  //myICM.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  bool initialized = false;
  while (!initialized)
  {
    myICM.begin(WIRE_PORT, AD0_VAL);


    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
}

std::string get_imu_values() {
  if (myICM.dataReady())
  {
    myICM.getAGMT();         // The values are only updated when you call 'getAGMT'
                             //    printRawAGMT( myICM.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
    return printScaledAGMT(&myICM); // This function takes into account the scale settings from when the measurement was made to calculate the values with units
  }
  else
  {
    return "IMU waiting for data";
  }
}