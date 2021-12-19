# embedded_controls




Instructions:
1. [for updating the micro ros agent] install micro ros - https://micro.ros.org/docs/tutorials/core/teensy_with_arduino/
    * everything here was already downloaded
2. build and flash teensy from platformio
3. 'colcon build'
4. run micro ros agent - `ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0`