// turn off all vectorization and asserts about array alignment. This is mostly important because any crashes 
// related to array alignment would be impossible to debug (where would the error message be printed?)

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include <vector>
#include <Eigen.h>

#include <eigen_utilities.h>

typedef Eigen::Matrix<double, 6, 1> Vector6d;


class TwistToMotor {
    public:
    TwistToMotor(std::vector<Vector6d> input_motor_vectors);

    Eigen::VectorXd wrench_to_motor_thrust(Eigen::VectorXd requested_wrench);

    Eigen::VectorXd motor_thrust_to_wrench(Eigen::VectorXd requested_wrench);

    // private:

    Eigen::MatrixXd motor_basis;
    Eigen::MatrixXd motor_basis_pinv;

    /*
        Converts a motor's position (<x_coord, y_coord, z_coord, x_component, y_component, z_component>) to the wrench produced by the motor
    */
    Vector6d motor_to_wrench(Vector6d motor_position_vect);
};
