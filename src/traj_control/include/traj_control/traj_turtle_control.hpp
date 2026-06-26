#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

using namespace std::chrono_literals;

struct Goal {
    double x;
    double y;
};

class TurtleControl : public rclcpp::Node {
public:
    TurtleControl();

private:
    void update_pose(const turtlesim::msg::Pose::SharedPtr msg);

    double euclidean_distance(double goal_x, double goal_y);

    double steering_angle(double goal_x, double goal_y);

    void control_loop(); 

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscriber_;
    rclcpp::TimerBase::SharedPtr timer_;

    turtlesim::msg::Pose current_pose_;
    std::vector<Goal> goal_queue_;
    double tolerance_;
};
