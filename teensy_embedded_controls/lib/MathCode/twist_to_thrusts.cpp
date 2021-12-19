#include <vector>
#include <eigen.h>

#include <eigen_utilities.h>

#include <twist_to_thrusts.h>

TwistToMotor::TwistToMotor(std::vector<Vector6d> input_motor_vectors) {

    // motor_basis = np.array([motor_to_wrench(i) for i in motor_placements]).T

    int mat_rows = 6;
    int mat_columns = input_motor_vectors.size();

    motor_basis = Eigen::Matrix<double, 6, Eigen::Dynamic>(mat_rows, mat_columns);
    for (int col = 0; col < mat_columns; col++) {
        motor_basis.col(col) = motor_to_wrench(input_motor_vectors.at(col));
        // motor_basis.col(col) = input_motor_vectors.at(col);
    }

    motor_basis = motor_basis.eval().transpose().eval();  // eval() needed to prevent aliasing

    motor_basis_pinv = pseudoInverse(motor_basis);
}

Eigen::VectorXd TwistToMotor::wrench_to_motor_thrust(Eigen::VectorXd requested_wrench) {
    // requested_wrench_in_motor_basis = np.linalg.pinv(motor_basis).dot(requested_wrench)  # use pseudoinverse, don't assume that the motors form a basis
    Eigen::VectorXd output = motor_basis_pinv * requested_wrench;
    return output;
}

Eigen::VectorXd TwistToMotor::motor_thrust_to_wrench(Eigen::VectorXd requested_wrench) {
    // requested_wrench_in_motor_basis = np.linalg.pinv(motor_basis).dot(requested_wrench)  # use pseudoinverse, don't assume that the motors form a basis
    Eigen::VectorXd output = motor_basis * requested_wrench;
    return output;
}

Vector6d TwistToMotor::motor_to_wrench(Vector6d motor_position_vect) {
    Eigen::Vector3d _0_3 = motor_position_vect.block(0, 0, 3, 0);
    Eigen::Vector3d _3_6 = motor_position_vect.block(3, 0, 3, 0);

    Eigen::Vector3d force;
    Eigen::Vector3d moment;
    // // the force component of the wrench is equal to the motor axis
    // force = motor[3:6]
    force = _3_6;

    // force = force / np.linalg.norm(force)  # normalize the force axis vector
    force = force / force.eval().norm();

    // // the torque is a little trickier. The cross product can be used for this
    // moment = np.cross(motor[0:3], motor[3:6])
    moment = _0_3.cross(_3_6);

    // return np.concatenate((force, moment))
    Vector6d wrench;
    wrench << force, moment;
    return wrench;
}
