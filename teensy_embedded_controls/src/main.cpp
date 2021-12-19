#include <micro_ros_arduino.h>

#include <vector>
#include <string>
#include <functional>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

// nodelets will add themselves to these vectors (since all subscribers and publishers need to be initialized on startup)


struct timer_registration {
  rcl_timer_t timer_obj;
  int period_ms;
  rcl_timer_callback_t callback;
};


// micro_ros timer object, period (ms), timer callback (recieves timer_object, last_call_time as arguments)
// std::vector<std::tuple<rcl_timer_t, int, std::function<void(rcl_timer_t*, int64_t)>>> component_timers;
std::vector<timer_registration> component_timers;


struct subscription_registration {
  rcl_subscription_t subscription_obj;
  std::string topic_name; 
  rosidl_message_type_support_t* message_type;
  void* allocated_message;
  rclc_subscription_callback_t callback;
};

// micro_ros subscription object, topic name, message type, callback function pointer, best effort/reliable
//  (true is best effort, false is reliable)
std::vector<subscription_registration> component_subscribers;

// add timers and subscriptions here
#include "IMU_read.h"
const unsigned int imu_read_timer_period = 250;
rcl_timer_t imu_read_time;

void setup_timers() {
  component_timers.push_back((timer_registration){imu_read_time, imu_read_timer_period, imu_read_timer_callback});

}


void setup_subscriptions() {

}



#include <Arduino.h>

// #include <twist_to_thrusts.h>



// rcl_subscription_t subscriber;
// std_msgs__msg__Int32 msg;

// rcl_publisher_t publisher_internal;

// rcl_subscription_t subscriber_internal;
// std_msgs__msg__Int32 msg_internal;

rcl_publisher_t publisher;

// rcl_timer_t timer;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;


#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}


// void subscription_callback(const void * msgin)
// {
//   const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
//   digitalWrite(LED_PIN, (msg->data == 0) ? LOW : HIGH);
  
//   RCCHECK(rcl_publish(&publisher_internal, msg, NULL));
//   // RCCHECK(rcl_publish(&publisher, msg, NULL));
// }


void subscription_callback_internal(const void * msgin)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  digitalWrite(LED_PIN, (msg->data == 0) ? LOW : HIGH);
  
  RCCHECK(rcl_publish(&publisher, msg, NULL));
}


void setup() {
  setup_timers();
  setup_subscriptions();

  set_microros_transports();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  
  
  // put this delay here so the host micro_ros agent has time to conenct
  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

//   // create subscriber
//   RCCHECK(rclc_subscription_init_best_effort(
// //  RCCHECK(rclc_subscription_init_default(
//     &subscriber,
//     &node,
//     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//     "/micro_ros_arduino_subscriber"));
//   num_handles++;  // SUPER IMPORTANT, DO NOT FORGET OR MICRO ROS WILL BREAK IN WEIRD WAYS

//   RCCHECK(rclc_publisher_init_best_effort(
//   // RCCHECK(rclc_publisher_init_default(
//     &publisher_internal,
//     &node,
//     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//     "/teensy_to_teensy"));



//   RCCHECK(rclc_subscription_init_best_effort(
// //  RCCHECK(rclc_subscription_init_default(
//     &subscriber_internal,
//     &node,
//     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//     "/teensy_to_teensy"));
//   num_handles++;

//   RCCHECK(rclc_publisher_init_best_effort(
// //  RCCHECK(rclc_publisher_init_default(
//     &publisher,
//     &node,
//     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//     "/micro_ros_arduino_node_publisher2"));

  //TODO: delete this stuff:

  // set up nodelet subscribers
  int num_handles = 0;
  for (timer_registration timer_struct : component_timers) {
    RCCHECK(rclc_timer_init_default(
      &timer_struct.timer_obj,
      &support,
      RCL_MS_TO_NS(timer_struct.period_ms),
      timer_struct.callback));
      num_handles += 1;
  }

  for (subscription_registration sub : component_subscribers) {
    RCCHECK(rclc_subscription_init_default(
      &sub.subscription_obj,
      &node,
      sub.message_type,
      sub.topic_name.c_str()));

      num_handles += 1;
  }



  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, num_handles, &allocator));
  // RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));
  // RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_internal, &msg_internal, &subscription_callback_internal, ON_NEW_DATA));

    for (subscription_registration sub : component_subscribers) {
      RCCHECK(rclc_executor_add_subscription(&executor, &sub.subscription_obj, sub.allocated_message, sub.callback, ALWAYS));
  }

}



void loop() {
//  delay(10);
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1)));

//  std_msgs__msg__Int32 msg;
//  msg.data = 1;
//  RCSOFTCHECK(rcl_publish(&publisher_heartbeat, &msg, NULL));

}
