#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
/*--------------------------------------------------------------------
* main()
* Main function to set up ROS node.
*------------------------------------------------------------------*/
int main(int argc, char **argv)
{
// Initialising ROS2
rclcpp::init(argc, argv);
// Shared Pointer to object of type rclcpp::Node
std::shared_ptr<rclcpp::Node> node =
rclcpp::Node::make_shared("velocity_publisher");
// Shared Pointer to rclcpp::Publisher
// Member function of Node creates the Publisher
std::shared_ptr<rclcpp::Publisher<geometry_msgs::msg::Twist>>
turtle_vel_pub =
node->create_publisher<geometry_msgs::msg::Twist>(
"/turtle1/cmd_vel", 10);
// Define a rate object (10 Hz)
rclcpp::Rate loop_rate(10);
// main loop
while (rclcpp::ok())
{
// Defining the message type
geometry_msgs::msg::Twist vel_msg;
vel_msg.linear.x = 0.5;
vel_msg.angular.z = 0.2;
// Publishing the message
turtle_vel_pub->publish(vel_msg);
// Print out the messages via the logger of the node
RCLCPP_INFO(node->get_logger(),
"Publisher turtlevelocity command[%0.2f m/s, %0.2f rad/s]",
vel_msg.linear.x, vel_msg.angular.z);
// Processing callbacks
rclcpp::spin_some(node);
// Sleep as long as required to maintain loop rate
loop_rate.sleep();
}
// Shutdown ROS2 node
rclcpp::shutdown();
return 0;
} // end main()
