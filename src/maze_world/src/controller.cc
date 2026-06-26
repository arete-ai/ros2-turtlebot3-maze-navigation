#include <chrono>
#include <functional>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float32.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include <string.h>
#include <cmath>

using std::placeholders::_1;
using namespace std::chrono_literals;

// Class for controlling the turtlebot inside the maze
class MazeController : public rclcpp::Node {
  private:
    std::shared_ptr<rclcpp::Publisher<geometry_msgs::msg::Twist> > turtle_pub_;  
    rclcpp::TimerBase::SharedPtr timer_;
    //std::shared_ptr<rclcpp::Subscription<nav_msgs::msg::Odometry> > odom_sub_;  
    //std::shared_ptr<rclcpp::Subscription<sensor_msgs::msg::LaserScan> > scan_sub_;  
    int max_dist = 0;

    // Array that stores a simple trajectory consisting of SIZE pieces
    static unsigned const int SIZE = 4; 
    //iterations lin_vel ang_vel 
    double steps[SIZE][3] = {
                      {5,0.1,0.0},
                      {5,0.0,0.0},
                      {5,0.0,0.5},
                      {5,0.0,0.0},
    };
  public:
    MazeController() : Node("mazecontroller") {
      // Publisher to move the robot forward
      turtle_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 1000);
      // Publisher is used inside the timer function that is called every 100 ms
      timer_ = this->create_wall_timer(100ms, std::bind(&MazeController::timer_callback, this)); 
      // Subscriber for odometry data
      //odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>("/odom", 1000, 
      //std::bind(&MazeController::odomCallback, this, _1));
      // Subscriner for laser scan data
      //scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", 100, 
      //std::bind(&MazeController::getMaxRange, this, _1));
    }

    /**
     * Example code to move the robot forward simply reading the preprogrammed commands from 
     * the array steps.
     */
    void timer_callback() {
      static unsigned int s = 0;
      static unsigned int i = 0;
      if(s < SIZE && rclcpp::ok()) {
        geometry_msgs::msg::Twist msg;
        //turtle_pub_->publish(msg);
        i++;
        if(i >= steps[s][0]) {
          i=0;
          s++;
        }
        RCLCPP_INFO(this->get_logger(), "Max_range: [%f %f %f]", steps[s][0], steps[s][1], steps[s][2]);
      }
    }

    /**
     * Example code to process the laser scan messages
     */
    void getMaxRange(const sensor_msgs::msg::LaserScan& scan) {
      unsigned int size = scan.ranges.size();
      float tmp_max = scan.range_min;
      int tmp_index;

      // Loop through all ranges
      for (unsigned int i = 0; i < size; i++) {
        if(scan.ranges[i] > tmp_max && !std::isinf(scan.ranges[i])) {
          tmp_max = scan.ranges[i];
          tmp_index = i;
        }
      }
      max_dist = tmp_max;     // store in global variable
      RCLCPP_INFO(this->get_logger(), "Max_range: [%d %f]", tmp_index, tmp_max);
    }

    /**
      * Example code to get the current pose of the robot from odometry
      */
    void odomCallback(const nav_msgs::msg::Odometry & msg) {
      tf2::Quaternion tf2_quat;
      tf2::fromMsg(msg.pose.pose.orientation, tf2_quat);
      double roll, pitch, yaw;
      tf2::Matrix3x3(tf2_quat).getRPY(roll, pitch, yaw);
      RCLCPP_INFO(this->get_logger(), "I am at: [%f %f %f]",
      msg.pose.pose.position.x, msg.pose.pose.position.y, yaw);
    }
};

// main function
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  // Creates a MazeController
  rclcpp::spin(std::make_shared<MazeController>());
  rclcpp::shutdown(); 
  return 0;
}
