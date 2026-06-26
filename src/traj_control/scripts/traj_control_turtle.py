#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose
from math import sqrt, atan2

class TurtleControl(Node):

    def __init__(self):
        # Initialize the node with the name 'simple_turtle_controller'
        super().__init__('simple_turtle_controller')
        
        # Publisher which will publish to the topic '/turtle1/cmd_vel'
        self.velocity_publisher = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)
        
        # Subscriber to the topic '/turtle1/pose'
        self.pose_subscriber = self.create_subscription(Pose, '/turtle1/pose', self.update_pose, 10)
        
        self.pose = Pose()
        self.goal_queue = [(1.0, 3.0), (5.0, 2.0), (2.0, 10.0)]
        self.tolerance = 0.1
        
        # Timer for the control loop (10Hz)
        self.timer = self.create_timer(0.1, self.control_loop)
        
        self.get_logger().info('ROS2 Turtle Controller initialized')
    
    def update_pose(self, data):
        """Callback function for pose updates."""
        self.pose = data
        # Optional: round values for cleaner logs
        # self.pose.x = round(self.pose.x, 4)

    def euclidean_distance(self, goal_x, goal_y):
        """Euclidean distance between current pose and the goal."""
        return sqrt(pow(goal_x - self.pose.x, 2) + pow(goal_y - self.pose.y, 2))

    def linear_vel(self, goal_x, goal_y, constant=1.5):
        return constant * self.euclidean_distance(goal_x, goal_y)

    def angular_vel(self, goal_x, goal_y, constant=6.0):
        return constant * (self.steering_angle(goal_x, goal_y) - self.pose.theta)
    
    def steering_angle(self, goal_x, goal_y):
        return atan2(goal_y - self.pose.y, goal_x - self.pose.x)

    def control_loop(self):
        """Main control loop triggered by the timer."""
        if not self.goal_queue:
            self.get_logger().info('All goals reached!')
            self.timer.cancel()
            return

        goal_x, goal_y = self.goal_queue[0]
        
        if self.euclidean_distance(goal_x, goal_y) >= self.tolerance:
            vel_msg = Twist()
            
            # Linear velocity in the x-axis
            vel_msg.linear.x = self.linear_vel(goal_x, goal_y)
            
            # Angular velocity in the z-axis
            vel_msg.angular.z = self.angular_vel(goal_x, goal_y)
            
            self.get_logger().info(f'Setting linear to {vel_msg.linear.x} and angular to {vel_msg.angular.z}!')
            self.velocity_publisher.publish(vel_msg)
        else:
            # Goal reached, move to next goal
            self.get_logger().info(f'Goal {goal_x}, {goal_y} reached!')
            
            # Stop the robot
            stop_msg = Twist()
            self.velocity_publisher.publish(stop_msg)
            
            # Remove finished goal from queue
            self.goal_queue.pop(0)

def main(args=None):
    rclpy.init(args=args)
    try:
        controller = TurtleControl()
        rclpy.spin(controller)
    except KeyboardInterrupt:
        pass
    finally:
        controller.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()

