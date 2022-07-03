// DUMb Micro Ros - 
// Copyright © 2022-2999, David Michelman
// BSD 3 clause License

#include "dummr.h"
#include <Arduino.h>

#ifndef DUMMR_MESSAGE_JSON_SIZE
#define DUMMR_MESSAGE_JSON_SIZE 256
#endif

StaticJsonDocument<128> dummr_last_topic;
StaticJsonDocument<256> dummr_last_message_data;

String start_marker = "dummr:";


static bool debug = false;

void log(String msg) {
  if (debug) {
    Serial.print("debug:");
    Serial.println(msg);
  }
}


StaticJsonDocument<128> get_last_topic_json() {
    return dummr_last_topic;
}

StaticJsonDocument<256> get_last_message_json() {
    return dummr_last_message_data;
}


void dummr_init() {
  // there might be partial data host serial buffer (if the uc was reset) in the
  // middle of printing a line. Add a newline to clear it
  SerialUSB1.println();
}


// returns false if the marker was not found
bool try_for_start_marker() {
  uint32_t timeout_deadline = millis() + 10;
  for (int i = 0; i < start_marker.length(); i++) {
    while ((!SerialUSB1.available()) && (millis() <= timeout_deadline)) {}
    if (millis() > timeout_deadline) {
      log("try_for_start_maker(): deadline is up");
      return false;
    }
    char next_char = SerialUSB1.peek();
    log("try_for_start_maker(): got char " + next_char);
    if (next_char == start_marker[i]) {
      SerialUSB1.read();
    }
    else if (next_char == start_marker[i]) {
      i = -1;
    }
    else {
      SerialUSB1.read();
      i = -1;
    }
  }
  return true;
}


void pub_message(const char* topic, const char* type, StaticJsonDocument<256> user_data) {
  const int capacity = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<capacity> doc;
  doc["op"] = "pub";
  doc["topic"] = topic;
  doc["type"] = type;

  SerialUSB1.print(start_marker);
  serializeJson(doc, SerialUSB1);
  SerialUSB1.write(",");
  serializeJson(user_data, SerialUSB1);
  SerialUSB1.write('\n');
}


void sub_message(const char* topic, const char* type) {
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> doc;
  doc["op"] = "sub";
  doc["topic"] = topic;
  doc["type"] = "std_msgs/String";

  SerialUSB1.print(start_marker);
  serializeJson(doc, SerialUSB1);
  SerialUSB1.write('\n');
}


bool dummr_check_for_msg() {
  SerialUSB1.print(start_marker);
  SerialUSB1.println("fetch");
  
  if (try_for_start_marker()) {
    while (!SerialUSB1.available()) {}
    char command_char = SerialUSB1.read();
    if (command_char == 'm') {
      
      DeserializationError error1 = deserializeJson(dummr_last_topic, SerialUSB1);
      DeserializationError error2 = deserializeJson(dummr_last_message_data, SerialUSB1);
      if (error1) {
        SerialUSB1.print(F("deserializeJson() failed: "));
        SerialUSB1.println(error1.f_str());
        return false;
      }
      if (error2) {
        SerialUSB1.print(F("deserializeJson() failed: "));
        SerialUSB1.println(error2.f_str());
        return false;
      }
      return true;
    }
  }
  return false;
}

