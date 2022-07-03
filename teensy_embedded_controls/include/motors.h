#pragma once

#include <Arduino.h>
#include <Servo.h>
#include <vector>
#include <array>


std::vector<Servo> motors;


std::array<int, 6> motor_pin_list = {2, 3, 4, 5, 6, 8};
std::array<int, 6> motor_values = {2, 3, 4, 5, 6, 8};


bool motors_enabled = false;

bool motor_debug = false;


void init_motors() {

    for (int motor_pin : motor_pin_list) {
        motors.emplace_back(Servo());
        motors.back().attach(motor_pin);
        motors.back().write(70);
    }

    delay(500);

    for (auto motor : motors) {
        motor.write(70);
    }

    delay(500);

    for (auto motor : motors) {
        motor.write(90);
    }

    pinMode(23, INPUT);
}


void loop_motors() {
    bool new_motors_enabled = digitalReadFast(23) == 0;

    if (new_motors_enabled && (!motors_enabled)) {
        // motors were just enabled
        for (int i = 0; i < 6; i++) {
            motors.at(i).write(motor_values.at(i));
        }
    }
    else if ((!new_motors_enabled) && motors_enabled) {
        // motors were just enabled
        for (int i = 0; i < 6; i++) {
            motors.at(i).write(0);
        }
    }

    motors_enabled = new_motors_enabled;
}


void spin_motor(int motor_num, float power) {
    float servo_val = map(power, -1, 1, 0, 180);
    servo_val = max(20, min(160, servo_val));
    if (motor_debug) {
        Serial.print("spinning at ");
        Serial.print(power);
        Serial.print(" which equates to ");
        Serial.println(servo_val);
    }

    motor_values.at(motor_num) = servo_val;

    if (motors_enabled) {
        motors.at(motor_num).write(servo_val);
    }
}


long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float min(float a, float b) {
    if (a < b) {
        return a;
    }
    else {
        return b;
    }
}

float max(float a, float b) {
    if (a > b) {
        return a;
    }
    else {
        return b;
    }
}

