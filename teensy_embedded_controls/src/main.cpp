// #include <vector>
// #include <string>
// #include <functional>

// #include <stdio.h>

#include <Arduino.h>

#include <Servo.h>

#include <dummr.h>

#include <IMU_read.h>


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
    // turn the LED on to prove life
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);  

    dummr_init();
    setup_subscriptions();
}



void loop() {

    if (dummr_check_for_msg()) {
        String topic = String((const char*) dummr_last_topic["topic"]);

        if (topic.equals("/teensy/input")) {
        // use the arduinojson assistant to figure out how big StaticJsonDocument should be - https://arduinojson.org/v6/assistant/
        StaticJsonDocument<256> return_doc;
        return_doc["data"] = dummr_last_message_data["data"]; 
        pub_message("/teensy/echo", "std_msgs/String", return_doc);
        }
    }

    std::string imu_reading = get_imu_values();

    StaticJsonDocument<2048> imu_doc;
    imu_doc["data"] = dummr_last_message_data["data"]; 
    pub_message("/teensy/imu_reading", "std_msgs/String", imu_doc);


    digitalWrite(LED_PIN, HIGH);
    delay(500);

    digitalWrite(LED_PIN, LOW);
    delay(500);

}
