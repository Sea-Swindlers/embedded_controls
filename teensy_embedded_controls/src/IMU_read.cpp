#include "IMU_read.h"


#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>


#include <Arduino.h>



#define LED_PIN 13

void imu_read_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	printf("Last callback time: %ld\n", last_call_time);

    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

	if (timer != NULL) {
		// Perform actions
		asm("nop");
	}
}
