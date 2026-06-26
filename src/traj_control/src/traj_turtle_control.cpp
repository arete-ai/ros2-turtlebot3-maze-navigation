#include "traj_control/traj_turtle_control.hpp"
#include <cmath>

TurtleControl::TurtleControl() : Node("simple_turtle_controller") {
    // Publisher for velocity - Name exakt wie im Header
    velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    // Subscriber for pose - Name exakt wie im Header
    pose_subscriber_ = this->create_subscription<turtlesim::msg::Pose>(
        "/turtle1/pose", 10, std::bind(&TurtleControl::update_pose, this, std::placeholders::_1));

    // Define goals 
    goal_queue_ = {{1.0, 3.0}, {5.0, 2.0}, {2.0, 10.0},{5.5, 5.5}};
    tolerance_ = 0.1;

    // Control Loop Timer (10Hz)
    timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&TurtleControl::control_loop, this));

    RCLCPP_INFO(this->get_logger(), "C++ Turtle Control started.");
}

void TurtleControl::update_pose(const turtlesim::msg::Pose::SharedPtr msg) {
    // Aktuelle Pose in current_pose_ speichern
    current_pose_ = *msg; 
}

double TurtleControl::euclidean_distance(double goal_x, double goal_y) {
    return std::sqrt(std::pow(goal_y - current_pose_.y, 2) + std::pow(goal_x - current_pose_.x, 2));
}

double TurtleControl::steering_angle(double goal_x, double goal_y) {
    return std::atan2(goal_y - current_pose_.y, goal_x - current_pose_.x);
}

/**
 * Member function that is called every time timer runs up
 */
void TurtleControl::control_loop() {
    // SICHERHEITS-CHECK: Warten, bis die allerersten Daten empfangen wurden
    if (current_pose_.x == 0.0 && current_pose_.y == 0.0) {
        return;
    }

    if (goal_queue_.empty()) {
        RCLCPP_INFO_ONCE(this->get_logger(), "All goals reached!");
        timer_->cancel();
        return;
    }

    Goal current_goal = goal_queue_.front();
      
    // Compute distance
    double distance = euclidean_distance(current_goal.x, current_goal.y);

    // If goal not reached
    if (distance > tolerance_) {  
        geometry_msgs::msg::Twist cmd_vel;

        // P-controller for linear velocity
        double kv = 1.5;
        cmd_vel.linear.x = kv * distance;

        // P-controller for angular velocity
        double kw = 6.0;
        double target_angle = steering_angle(current_goal.x, current_goal.y);
        double angle_diff = target_angle - current_pose_.theta;

        // Normalise angle difference to be between -M_PI and M_PI to avoid unneccessary rotation
        while (angle_diff > M_PI) angle_diff -= 2.0 * M_PI;
        while (angle_diff < -M_PI) angle_diff += 2.0 * M_PI;

        cmd_vel.angular.z = kw * angle_diff;

        // Publish message
        velocity_publisher_->publish(cmd_vel);

    } else {
        RCLCPP_INFO(this->get_logger(), "Goal reached: x=%.2f, y=%.2f", current_goal.x, current_goal.y);
        
        // Stop the robot
        geometry_msgs::msg::Twist stop_msg;
        stop_msg.linear.x = 0.0;
        stop_msg.angular.z = 0.0;
        velocity_publisher_->publish(stop_msg);

        // Next goal
        goal_queue_.erase(goal_queue_.begin());
    }
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  TurtleControl::SharedPtr node = std::make_shared<TurtleControl>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
