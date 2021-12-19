// run tests with this command: pio test -e teensy40 --without-testing && pio test -e teensy40 --without-building --without-uploading

#include <Arduino.h>
#include <unity.h>

#include <Eigen.h>
#include <Dense>
#include <eigen_utilities.h>


// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_identity_matrix(void) {
    Eigen::MatrixXd i = Eigen::MatrixXd::Identity(4, 4);
    Eigen::MatrixXd inv = pseudoInverse(i);
    TEST_ASSERT_EQUAL(0, (i - inv).norm());
}

void test_invertable_2x2(void) {
    Eigen::Matrix2d testMat;
    testMat << 1, 2, 3, 4;
    Eigen::MatrixXd inv = pseudoInverse(testMat);
    TEST_ASSERT_EQUAL(0, (testMat * inv - Eigen::MatrixXd::Identity(2, 2)).norm());
}

void test_3x2(void) {
    Eigen::Matrix<double, 3, 2> testMat;
    testMat << 1, 2, 3, 4, 5, 6;
    Eigen::MatrixXd inv = pseudoInverse(testMat);
    TEST_ASSERT_EQUAL(0, (testMat * inv - Eigen::MatrixXd::Identity(2, 2)).norm());
}

void test_2x3(void) {
    Eigen::Matrix<double, 2, 3> testMat;
    testMat << 1, 2, 3, 4, 5, 6;
    Eigen::MatrixXd inv = pseudoInverse(testMat);
    TEST_ASSERT_EQUAL(0, (inv * testMat - Eigen::MatrixXd::Identity(2, 2)).norm());
}

void test_singular_2x2(void) {
    Eigen::Matrix<double, 2, 2> testMat;
    testMat << 0, 0, 0, 1;
    Eigen::MatrixXd inv = pseudoInverse(testMat);
    TEST_ASSERT_EQUAL(0, (inv * testMat - Eigen::MatrixXd::Identity(2, 2)).norm());
}


void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(4000);

    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_identity_matrix);
    RUN_TEST(test_invertable_2x2);
    RUN_TEST(test_3x2);
    RUN_TEST(test_2x3);
    RUN_TEST(test_singular_2x2);

    UNITY_END(); // stop unit testing
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop() {
    // if (i < max_blinks)
    // {
    //     RUN_TEST(test_identity_matrix);
    //     delay(1000);
    //     i++;
    // }
    // else if (i == max_blinks) {
    //   UNITY_END(); // stop unit testing
    // }
}
