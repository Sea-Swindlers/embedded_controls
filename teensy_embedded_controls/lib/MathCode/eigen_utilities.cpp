#include<eigen_utilities.h>
#include<Dense>


Eigen::MatrixXd pseudoInverse(const Eigen::MatrixXd& a) {
    Eigen::MatrixXd a_t = a.transpose();
	Eigen::MatrixXd inner = a_t * a;
	Eigen::MatrixXd inner_i = inner.inverse();
	Eigen::MatrixXd output = inner_i * a_t;
	return output;
}