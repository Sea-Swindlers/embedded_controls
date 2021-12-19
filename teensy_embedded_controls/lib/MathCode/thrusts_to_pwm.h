#include <eigen.h>
#include <vector>



class ThrustsToPwm {
    public:

    ThrustsToPwm() {
        // TODO: maybe get scalaing factors from ROS parameters? That might make unit tests harder to write?
        // then again this project as a whole won't work without ros anyways
    }

    std::vector<int> get_pwms(Eigen::VectorXd thrusts);

    private:



};