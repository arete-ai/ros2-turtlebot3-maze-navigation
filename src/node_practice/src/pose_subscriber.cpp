#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
// Callback function for the subscriber.
// In ROS 2, using a SharedPtr (Smart Pointer) as argument is preferred.
void poseCallback(const turtlesim::msg::Pose::SharedPtr msg)
{
// Print messages to the terminal.
// Since we don't have access to the Node object here,
// we use a static logger.
RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Turtle pose: x:%0.6f, y: %0.6f", msg->x, msg->y);
}
/*--------------------------------------------------------------------
* main()
* Main function to set up the ROS 2 node.
*------------------------------------------------------------------*/
int main(int argc, char **argv)
{
// Initialize ROS 2.
rclcpp::init(argc, argv);
// Shared pointer to a node.
std::shared_ptr<rclcpp::Node> node =
rclcpp::Node::make_shared("pose_subscriber");
// Member function of node creates a Subscription object
// and returns a shared pointer to it
// The message type is expected within the "msg" namespace.
std::shared_ptr<rclcpp::Subscription<turtlesim::msg::Pose>> pose_sub =
node->create_subscription<turtlesim::msg::Pose>("/turtle1/pose",
10, poseCallback);
// Main loop for processing callbacks.
rclcpp::spin(node);
// Properly shut down ROS 2.
rclcpp::shutdown();
return 0;
}
