// run tests with this command: pio test -e teensy40 --without-testing && pio test -e teensy40 --without-building --without-uploading

#include <string>

#include <Arduino.h>
#include <unity.h>

#include <Eigen.h>
#include <Dense>
#include <twist_to_thrusts.h>


// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

std::tuple<double, Eigen::MatrixXd> get_error(TwistToMotor& twist_to_motor, Vector6d& requested_wrench) {
    Eigen::MatrixXd motor_thrusts = twist_to_motor.wrench_to_motor_thrust(requested_wrench);
    Vector6d resulting_wrench = twist_to_motor.motor_to_wrench(motor_thrusts);
    double error = (requested_wrench - resulting_wrench).cwiseAbs().sum();
    return std::make_tuple(error, motor_thrusts);
}


// from https://stackoverflow.com/questions/37508017/how-to-convert-eigen-librarys-matrix-or-vector-types-to-string
static std::string matToString(const Eigen::MatrixXd& mat){
    std::string output = "vector: ";
    std::vector<char> temp_vector(100);
    for(int j = 0; j < mat.cols(); j++) {
        output = output + "<";
        for(int i = 0; i < mat.rows(); i++) {
            sprintf(&temp_vector[0], "%f, ", mat(i, j));
            output += std::string(&temp_vector[0]);
        }
        output = output + "> ";
    }
    return output;
}


void test_single_motor(void) {
    Vector6d motor;
    motor << 1, 0, 0, 1, 0, 0;
    std::vector<Vector6d> motors = {motor};
    
    TwistToMotor twist_to_motor(motors);

    std::vector<char> text_buffer(10000);   // 10k characters should be enough. 
    // Since we're on a teensy 4.0 we have 1mb of ram so little chance of running out because of this

    Vector6d test_wrench;
    test_wrench << 1, 0, 0, 0, 0, 0;
    Eigen::Matrix<double, 1, 1> expected_motor_thrusts;
    expected_motor_thrusts << 1;

    Eigen::Matrix<double, 1, 1> output_motor_powers = twist_to_motor.wrench_to_motor_thrust(test_wrench);
    Eigen::Matrix<double, 1, 1> diff = output_motor_powers - expected_motor_thrusts;

    TEST_ASSERT_EQUAL(0, diff(0, 0));
}

void test_three_motors(void) {
    Vector6d motor1, motor2, motor3;
    motor1 << 1, 0, 0, 1, 0, 0;
    motor2 << 0, 1, 0, 0, 1, 0;
    motor3 << 0, 0, 1, 0, 0, 1;
    std::vector<Vector6d> motors = {motor1, motor2, motor3};
    
    TwistToMotor twist_to_motor(motors);

    Vector6d test_wrench;
    double error = 0;
    Eigen::MatrixXd motor_thrusts;

    Vector6d a;
    a << 1, 2, 3, 4, 5, 6;
    Vector6d b = twist_to_motor.motor_to_wrench(a);


    test_wrench << 1, 0, 0, 0, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());

    test_wrench << 0, 1, 0, 0, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
    
    test_wrench << 0, 0, 1, 0, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
        
    test_wrench << 1, -1, 1, 0, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
        
    test_wrench << 0, 0, 0, 1, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
        
    test_wrench << 0, 0, 0, 0, 1, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());

    test_wrench << 0, 0, 0, 0, 0, 1;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
}


void test_bluerov_plus_1(void) {
    // this arangement should be holonomic
    Vector6d motor1, motor2, motor3, motor4, motor5, motor6, motor7;
    // square of forward/back motors
    motor1 <<  1,  1, 0, -1,  1, 0;
    motor2 << -1,  1, 0,  1,  1, 0;
    motor3 <<  1, -1, 0,  1,  1, 0;
    motor4 << -1, -1, 0, -1,  1, 0;
    // triangle of upward facing motors
    motor5 <<  1,  0, 1, 0, 0, 1;
    motor6 << -1,  0, 1, 0, 0, 1;
    motor7 <<  0, -1, 1, 0, 0, 1;
    std::vector<Vector6d> motors = {motor1, motor2, motor3, motor4, motor5, motor6};
    
    TwistToMotor twist_to_motor(motors);

    Vector6d test_wrench;
    double error = 0;
    Eigen::Matrix<double, Eigen::Dynamic, 1> motor_thrusts;

    test_wrench << 1, 0, 0, 0, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());

    test_wrench << 0, 1, 0, 0, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
    
    test_wrench << 0, 0, 1, 0, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
        
    test_wrench << 0, 0, 0, 1, 0, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
        
    test_wrench << 0, 0, 0, 0, 1, 0;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());

    test_wrench << 0, 0, 0, 0, 0, 1;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());

    test_wrench << 1, 1, 1, 1, 1, 1;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
    
    test_wrench << 1, -1, 1, -1, 1, -1;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
        
    test_wrench << 0.5, 11, 23, -6, 2, -4;
    std::tie(error, motor_thrusts) = get_error(twist_to_motor, test_wrench);
    TEST_ASSERT_EQUAL_MESSAGE(0, error, matToString(motor_thrusts).c_str());
}



void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(3000);

    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_three_motors);
    RUN_TEST(test_single_motor);
    RUN_TEST(test_bluerov_plus_1);

    UNITY_END(); // stop unit testing
}


uint8_t i = 0;
uint8_t max_blinks = 1;
void loop() {
    // if (i < max_blinks)
    // {
    //     RUN_TEST(test_dummy);
    //     delay(1000);
    //     i++;
    // }
    // else if (i == max_blinks) {
    //     RUN_TEST(test_single_motor);
    //     UNITY_END(); // stop unit testing
    // }
}
