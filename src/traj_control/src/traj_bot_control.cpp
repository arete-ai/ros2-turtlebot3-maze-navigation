#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <cmath>
#include <vector>

using namespace std::chrono_literals;

struct Goal { double x; double y; };

class TrajBotControl : public rclcpp::Node {
public:
    TrajBotControl() : Node("traj_bot_controller") {
        // Publisher für echten Roboter auf /cmd_vel
        velocity_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        
        // Subscriber für Odometrie auf /odom
        odom_subscriber_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 10, std::bind(&TrajBotControl::odom_callback, this, std::placeholders::_1));

        // ZIELE ANPASSEN: Da der Roboter bei (0,0) startet, wählen wir kleine, erreichbare Ziele:
        // 1. Fahre 0.5m nach vorne
        // 2. Fahre schräg zu X=0.5, Y=0.5
        // 3. Kehre zum Startpunkt (0,0) zurück
        goal_queue_ = {{0.5, 0.0}, {0.5, 0.5}, {0.0, 0.0}};
        tolerance_ = 0.05; // 5 cm Toleranz
        pose_received_ = false;

        // ZYKLUSZEIT: 100ms (10 Hz). Kann hier für "ruhiges Verhalten" angepasst werden.
        timer_ = this->create_wall_timer(100ms, std::bind(&TrajBotControl::control_loop, this));
        RCLCPP_INFO(this->get_logger(), "C++ Bot Control started.");
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;

        // Umrechnung von Quaternion in Gier-Winkel (Yaw) nach Skript
        double qx = msg->pose.pose.orientation.x;
        double qy = msg->pose.pose.orientation.y;
        double qz = msg->pose.pose.orientation.z;
        double qw = msg->pose.pose.orientation.w;
        
        double siny_cosp = 2.0 * (qw * qz + qx * qy);
        double cosy_cosp = 1.0 - 2.0 * (qy * qy + qz * qz);
        current_theta_ = std::atan2(siny_cosp, cosy_cosp);

        pose_received_ = true; // Bestätigung, dass Odometrie da ist
    }

    void control_loop() {
        if (!pose_received_) return;

        if (goal_queue_.empty()) {
            RCLCPP_INFO_ONCE(this->get_logger(), "All goals reached!");
            timer_->cancel();
            return;
        }

        Goal current_goal = goal_queue_.front();
        double distance = std::sqrt(std::pow(current_goal.y - current_y_, 2) + std::pow(current_goal.x - current_x_, 2));

        if (distance > tolerance_) {
            geometry_msgs::msg::Twist cmd_vel;

            // REGELPARAMETER: Modifiziere Kw und Kv hier!
            double kv = 0.5; // Linearer Regler (Standard: 0.5)
            double kw = 1.5; // Angularer Regler (Standard: 1.5)

            // Linear-Geschwindigkeit berechnen und auf MAX 0.22 m/s begrenzen!
            double linear_v = kv * distance;
            if (linear_v > 0.22) linear_v = 0.22;
            cmd_vel.linear.x = linear_v;

            // Angular-Geschwindigkeit berechnen
            double target_angle = std::atan2(current_goal.y - current_y_, current_goal.x - current_x_);
            double angle_diff = target_angle - current_theta_;

            while (angle_diff > M_PI) angle_diff -= 2.0 * M_PI;
            while (angle_diff < -M_PI) angle_diff += 2.0 * M_PI;

            cmd_vel.angular.z = kw * angle_diff;
            velocity_publisher_->publish(cmd_vel);

        } else {
            RCLCPP_INFO(this->get_logger(), "Goal reached: x=%.2f, y=%.2f", current_goal.x, current_goal.y);
            geometry_msgs::msg::Twist stop_msg;
            velocity_publisher_->publish(stop_msg);
            goal_queue_.erase(goal_queue_.begin());
        }
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscriber_;
    rclcpp::TimerBase::SharedPtr timer_;

    bool pose_received_;
    double current_x_;
    double current_y_;
    double current_theta_;
    std::vector<Goal> goal_queue_;
    double tolerance_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TrajBotControl>());
    rclcpp::shutdown();
    return 0;
}
