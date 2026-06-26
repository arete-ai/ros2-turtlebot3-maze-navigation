#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
// Callback function for the subscriber.
void velocityCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
// Print messages to the terminal.
// Since we don't have access to the Node object here,
// we use a static logger.
RCLCPP_INFO(rclcpp::get_logger("rclcpp"),
"Turtle velocity: [%0.2f m/s, %0.2f rad/s]",
msg->linear.x, msg->angular.z);
}
int main(int argc, char **argv)
{
rclcpp::init(argc, argv);
std::shared_ptr<rclcpp::Node> node =
rclcpp::Node::make_shared("pose_subscriber");
// Subscribe to /turtle1/cmd_vel instead
std::shared_ptr<rclcpp::Subscription<geometry_msgs::msg::Twist>> vel_msg
= node->create_subscription<geometry_msgs::msg::Twist>("/cmd_vel",
10, velocityCallback);
rclcpp::spin(node);
rclcpp::shutdown();
return 0;
}
