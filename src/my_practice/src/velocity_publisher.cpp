#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
int main(int argc, char **argv)
{
rclcpp::init(argc, argv);
std::shared_ptr<rclcpp::Node> node =
rclcpp::Node::make_shared("velocity_publisher");
std::shared_ptr<rclcpp::Publisher<geometry_msgs::msg::Twist>>
turtle_vel_pub =
node->create_publisher<geometry_msgs::msg::Twist>(
"/cmd_vel", 10);
rclcpp::Rate loop_rate(10);
while (rclcpp::ok())
{
geometry_msgs::msg::Twist vel_msg;
vel_msg.linear.x = 0.3;
vel_msg.angular.z = 0.2;
turtle_vel_pub->publish(vel_msg);
RCLCPP_INFO(node->get_logger(),
"Publisher turtlevelocity command[%0.2f m/s, %0.2f rad/s]",
vel_msg.linear.x, vel_msg.angular.z);
rclcpp::spin_some(node);
loop_rate.sleep();
}
rclcpp::shutdown();
return 0;
}
