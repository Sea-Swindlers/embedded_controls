// DUMb Micro Ros - 
// Copyright © 2022-2999, David Michelman
// BSD 3 clause License


#pragma once

#include <ArduinoJsonv6.h>


void dummr_init();

// returns false if the marker was not found
bool try_for_start_marker();
void pub_message(const char* topic, const char* type, StaticJsonDocument<256> user_data);
void sub_message(const char* topic, const char* type);
bool dummr_check_for_msg();

StaticJsonDocument<128> get_last_topic_json();
StaticJsonDocument<256> get_last_message_json();
