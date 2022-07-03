// #include <vector>
// #include <string>
// #include <functional>

// #include <stdio.h>

#include <Arduino.h>
#include <usb_desc.h>
#include <dummr.h>

#include <IMU_read.h>
#include <motors.h>


/*


message foramt:
{
  "topic_name": "string",
  "message_type": "string",
  "data": [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
  ]
}



teensy publish:
imu_read

teensy_recieve:
motor_write
controls


*/


void setup_subscriptions() {
    // using this as a life check
    sub_message("/teensy/input", "std_msgs/String");
}

// #include <twist_to_thrusts.h>


#define LED_PIN 13


// x fast blinks, then a 1.5 second rest period
void error_loop(int error_code){
    pinMode(LED_PIN, OUTPUT);
    while(1){
        for (int i = 0; i < error_code; i++) {
            digitalWrite(LED_PIN, HIGH);
        delay(250);
        digitalWrite(LED_PIN, LOW);
        delay(250);
        }
        delay(1250);
    }
}


void setup() {
    // wait for a serial console to connect before printing stuff
    // while (!Serial.available()) {}
    // delay(500);

    SerialUSB1.printf("hello from usb 1");
    SerialUSB2.printf("hello from usb 2");
    
    // turn the LED on to prove life
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);  

    dummr_init();
    setup_subscriptions();
    setup_imu();
    init_motors();
}


void loop() {

    if (dummr_check_for_msg()) {
        String topic = String((const char*) get_last_topic_json()["topic"]);

        if (topic.equals("/teensy/input")) {
            // use the arduinojson assistant to figure out how big StaticJsonDocument should be - https://arduinojson.org/v6/assistant/
            StaticJsonDocument<256> return_doc;
            return_doc["data"] = get_last_message_json()["data"]; 
            pub_message("/teensy/echo", "std_msgs/String", return_doc);
        }
    }

    loop_motors();

    // std::string imu_reading = get_imu_values();
    publish_imu_values();


    for (int i = 0; i < 6; i++) {
        digitalWrite(LED_PIN, HIGH);
        spin_motor(i, 0.1);
        delay(1200);

        digitalWrite(LED_PIN, LOW);
        spin_motor(i, -0.1);
        delay(1200);

        spin_motor(i, 0);
        // delay(1200);
    }
}
