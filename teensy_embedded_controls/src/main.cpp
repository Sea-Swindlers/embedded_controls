
#include <Arduino.h>
#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>


// nodelets will add themselves to these vectors (since all subscribers and publishers need to be initialized on startup)

// micro_ros timer object, period (ms), timer callback (recieves timer_object, last_call_time as arguments)
std::vector<std::tuple<rcl_timer_t, int, void *(rcl_timer_t*, int64_t)>> component_timers;

// micro_ros subscription object, topic name, message type, callback function pointer, best effort/reliable
//  (true is best effort, false is reliable)
std::vector<std::tuple<rcl_subscription_t, std::string, rosidl_message_type_support_t*, void *(const void *msgin), bool>> component_subscribers;


// includes to files containing subscriptions or timers must be below here

#include <twist_to_thrusts.h>


// rcl_subscription_t subscriber;
// std_msgs__msg__Int32 msg;

// rcl_publisher_t publisher_internal;

// rcl_subscription_t subscriber_internal;
// std_msgs__msg__Int32 msg_internal;

// rcl_publisher_t publisher;

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


// void subscription_callback_internal(const void * msgin)
// {
//   const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
//   digitalWrite(LED_PIN, (msg->data == 0) ? LOW : HIGH);
  
//   RCCHECK(rcl_publish(&publisher, msg, NULL));
// }


void setup() {
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


  // set up nodelet subscribers
  int num_handles = 0;
  for (std::tuple<rcl_timer_t, int, void *(rcl_timer_t*, int64_t)> timer_tuple : component_timers) {
    RCCHECK(rclc_timer_init_default(
      &timer,
      &support,
      RCL_MS_TO_NS(timer_timeout),
      timer_callback));

  }



  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, num_handles, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_internal, &msg_internal, &subscription_callback_internal, ON_NEW_DATA));

}



void loop() {
//  delay(10);
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1)));

//  std_msgs__msg__Int32 msg;
//  msg.data = 1;
//  RCSOFTCHECK(rcl_publish(&publisher_heartbeat, &msg, NULL));

}
