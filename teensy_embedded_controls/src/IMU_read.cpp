#include <IMU_read.h>

#include <vector>
#include <string>
#include <functional>

#include <stdio.h>

#include <arduino.h>

#include "ICM_20948_lib/ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU

#include <dummr.h>



#define SERIAL_PORT Serial

#define WIRE_PORT Wire // Your desired Wire port.
#define AD0_VAL 1      // The value of the last bit of the I2C address.                
                       // On the SparkFun 9DoF IMU breakout the default is 1, and when 
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
    if (val < 0) {
        result += "-";
    }
    else {
        result += " ";
    }
    for (uint8_t indi = 0; indi < leading; indi++) {
        uint32_t tenpow = 0;
        if (indi < (leading - 1)) {
          tenpow = 1;
        }
        for (uint8_t c = 0; c < (leading - 1 - indi); c++) {
          tenpow *= 10;
        }
        if (aval < tenpow) {
          result += "0";
        }
        else {
          break;
        }
    }
    if (val < 0) {
        result += -val;
    }
    else {
        result += val;
    }

    return result;
}


std::string printScaledAGMT(ICM_20948_I2C *sensor)
{
    std::string result;
    // result += "Scaled. Acc (mg) [ ";
    // result += printFormattedFloat(sensor->accX(), 5, 2);
    // result += ", ";
    // result += printFormattedFloat(sensor->accY(), 5, 2);
    // result += ", ";
    // result += printFormattedFloat(sensor->accZ(), 5, 2);
    // result += " ], Gyr (DPS) [ ";
    // result += printFormattedFloat(sensor->gyrX(), 5, 2);
    // result += ", ";
    // result += printFormattedFloat(sensor->gyrY(), 5, 2);
    // result += ", ";
    // result += printFormattedFloat(sensor->gyrZ(), 5, 2);
    // result += " ], Mag (uT) [ ";
    // result += printFormattedFloat(sensor->magX(), 5, 2);
    // result += ", ";
    // result += printFormattedFloat(sensor->magY(), 5, 2);
    // result += ", ";
    // result += printFormattedFloat(sensor->magZ(), 5, 2);
    // result += " ], Tmp (C) [ ";
    // result += printFormattedFloat(sensor->temp(), 5, 2);
    // result += " ]\n";


    result += "Scaled. Acc (mg) [ ";
    result += std::string(String(sensor->accX(), 2).c_str());
    result += ", ";
    result += std::string(String(sensor->accY(), 2).c_str());
    result += ", ";
    result += std::string(String(sensor->accZ(), 2).c_str());
    result += " ], Gyr (DPS) [ ";
    result += std::string(String(sensor->gyrX(), 2).c_str());
    result += ", ";
    result += std::string(String(sensor->gyrY(), 2).c_str());
    result += ", ";
    result += std::string(String(sensor->gyrZ(), 2).c_str());
    result += " ], Mag (uT) [ ";
    result += std::string(String(sensor->magX(), 2).c_str());
    result += ", ";
    result += std::string(String(sensor->magY(), 2).c_str());
    result += ", ";
    result += std::string(String(sensor->magZ(), 2).c_str());
    result += " ], Tmp (C) [ ";
    result += std::string(String(sensor->temp(), 2).c_str());
    result += " ]\n";

    // printf("Scaled. Acc (mg) [ %5.2f, %5.2f, %5.2f ], Gyr (DPS) [ %5.2f, %5.2f, %5.2f ], Mag (uT) [ %5.2f, %5.2f, %5.2f ], Tmp (C) [ %5.2f ]\n", 
    //         sensor->accX(), sensor->accY(), sensor->accZ(), 
    //         sensor->gyrX(), sensor->gyrY(), sensor->gyrZ(), 
    //         sensor->magX(), sensor->magY(), sensor->magZ(), 
    //         sensor->temp());


    return result;
}

void setup_imu()
{

    WIRE_PORT.begin();
    WIRE_PORT.setClock(400000);

    myICM.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

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
    Serial.println("getting IMU values");

    if (myICM.dataReady())
    {
        myICM.getAGMT();         // The values are only updated when you call 'getAGMT'
        printRawAGMT( myICM.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
        std::string result = printScaledAGMT(&myICM); // This function takes into account the scale settings from when the measurement was made to calculate the values with units
        Serial.print("results: ");
        Serial.println(result.c_str());
        return result;
    }
    else
    {
        return "IMU waiting for data";
    }
}


void populate_message_header(DynamicJsonDocument& msg) {
    JsonObject header = msg.createNestedObject("header");
    header["stamp"] = millis();
    header["frame_id"] = "rov_frame";
}


void publish_imu_values() {
    if (myICM.dataReady())
    {
        myICM.getAGMT();  // The values are only updated when you call 'getAGMT'
        printRawAGMT( myICM.agmt );

        DynamicJsonDocument imu_doc(2048);
        populate_message_header(imu_doc);
        // imu_doc["data"] = dummr_last_message_data["data"];
        JsonArray orientation = imu_doc.createNestedArray("orientation");
        for (int i = 0; i < 4; i++) {
          orientation.add(0.0);
        }
        JsonArray orientation_cov = imu_doc.createNestedArray("orientation_covariance");
        for (int i = 0; i < 9; i++) {
          orientation_cov.add(0.0);
        }
        orientation_cov[0] = -1.0;

        JsonArray angular_velocity = imu_doc.createNestedArray("orientation");
        angular_velocity.add(myICM.agmt.gyr.axes.x);
        angular_velocity.add(myICM.agmt.gyr.axes.y);
        angular_velocity.add(myICM.agmt.gyr.axes.z);
        JsonArray angular_velocity = imu_doc.createNestedArray("orientation_covariance");
        for (int i = 0; i < 9; i++) {
          angular_velocity.add(0.0);
        }
        // TODO: estimate the actual covariance when that becomes necessary
        angular_velocity[0] = 0.1;
        angular_velocity[4] = 0.1;
        angular_velocity[8] = 0.1;

        JsonArray linear_acceleration = imu_doc.createNestedArray("orientation");
        linear_acceleration.add(myICM.agmt.acc.axes.x);
        linear_acceleration.add(myICM.agmt.acc.axes.y);
        linear_acceleration.add(myICM.agmt.acc.axes.z);
        JsonArray linear_acceleration = imu_doc.createNestedArray("orientation_covariance");
        for (int i = 0; i < 9; i++) {
          linear_acceleration.add(0.0);
        }
        // TODO: estimate the actual covariance when that becomes necessary
        linear_acceleration[0] = 0.1;
        linear_acceleration[4] = 0.1;
        linear_acceleration[8] = 0.1;

        pub_message("/teensy/imu_reading", "sensor_msgs/Imu", imu_doc);



        DynamicJsonDocument temp_message(2048);
        temp_message["temperature"] = myICM.agmt.tmp.val;
        JsonObject temp_header = temp_message.createNestedObject("header");
        temp_header["stamp"] = millis();
        temp_header["frame_id"] = "rov_frame";
        pub_message("/teensy/imu_temperature", "sensor_msgs/Temperature", temp_message);

    }
} 