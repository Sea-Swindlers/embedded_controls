
#include <thrusts_to_pwm.h>
#include <algorithm>
#include <vector>
#include <motor_thrust_to_pwm_mappings.h>

bool comparator(const std::pair<double, int>& first, const std::pair<double, int>& second) {
    return first.first < second.first;
}

std::vector<int> ThrustsToPwm::get_pwms(Eigen::VectorXd thrusts) {

    std::vector<int> output_pwm;

    for (int i = 0; i < thrusts.rows(); i++) {
        auto output = motor_thrust_mappings.lower_bound(thrusts(i));
        output_pwm.push_back(output->second);
    }

    return output_pwm;
}
