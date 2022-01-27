// DUMb Micro Ros - 
// Copyright © 2022-2999, David Michelman
// BSD 3 clause License


#pragma once

#include <Arduino.h>
#include <ArduinoJsonv6.h>

#ifndef DUMMR_MESSAGE_JSON_SIZE
#define DUMMR_MESSAGE_JSON_SIZE 256
#endif

StaticJsonDocument<128> dummr_last_topic;
StaticJsonDocument<256> dummr_last_message_data;

String start_marker = "dummr:";

void dummr_init() {
  // there might be partial data host serial buffer (if the uc was reset) in the
  // middle of printing a line. Add a newline to clear it
  Serial.println();
}

// returns false if the marker was not found
bool try_for_start_marker() {
  for (int i = 0; i < start_marker.length(); i++) {
    while (!Serial.available()) {}
    char next_char = Serial.peek();
    if (next_char == start_marker[i]) {
      Serial.read();
    }
    else if (next_char == start_marker[i]) {
      i = -1;
    }
    else {
      Serial.read();
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

  Serial.print(start_marker);
  serializeJson(doc, Serial);
  Serial.write(",");
  serializeJson(user_data, Serial);
  Serial.write('\n');
}


void sub_message(const char* topic, const char* type) {
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> doc;
  doc["op"] = "sub";
  doc["topic"] = topic;
  doc["type"] = "std_msgs/String";

  Serial.print(start_marker);
  serializeJson(doc, Serial);
  Serial.write('\n');
}


bool dummr_check_for_msg() {
  Serial.print(start_marker);
  Serial.println("fetch");
  
  if (try_for_start_marker()) {
    while (!Serial.available()) {}
    char command_char = Serial.read();
    if (command_char == 'm') {
      
      DeserializationError error1 = deserializeJson(dummr_last_topic, Serial);
      DeserializationError error2 = deserializeJson(dummr_last_message_data, Serial);
      if (error1) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error1.f_str());
        return false;
      }
      if (error2) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error2.f_str());
        return false;
      }
      return true;
    }
  }
  return false;
}

