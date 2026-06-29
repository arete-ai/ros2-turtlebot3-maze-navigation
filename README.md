# ROS2 Practicum

This repository contains the source code and documentation for the Robotics Practicum at the THWS. 

The course introduces the Robot Operating System (ROS 2) and covers everything from basic node communication to implementing position control for a mobile robot (TurtleBot3).

<img width="65%" height="65%" src="https://github.com/user-attachments/assets/6145400d-5b17-4f78-a602-37b6b8534cb8" />

## Prerequisites

- **OS:** Ubuntu 22.04 
- **ROS Version:** ROS 2 Humble
- **Hardware/Simulation:** TurtleBot3 (Burger/Waffle_pi)

## Overview of the Experiments 

The practicum is divided into four main experiments:

### Experiment 1: Introduction to ROS
- **Topics Covered:** Basic concepts of ROS 2, the ROS file system, computation graph (Nodes, Topics, Messages), and essential command-line tools.
- **Focus:** Setting up the environment, booting Ubuntu, and understanding how ROS nodes communicate.

### Experiment 2: Programming with ROS
- **Topics Covered:** Understanding the `colcon` build system and workspace structure.
- **Focus:** Creating ROS 2 packages, configuring `package.xml` and `CMakeLists.txt`, and writing basic C++ Publisher and Subscriber nodes to exchange data.

### Experiment 3: Controlling a Mobile Robot via ROS
- **Topics Covered:** Interaction with the TurtleBot3 hardware/simulation. Processing Odometry (`nav_msgs/Odometry`) and LaserScan data.
- **Focus:** Developing a ROS node (`controller.cc`) to autonomously navigate the TurtleBot3 from a start position through a predefined maze avoiding obstacles.

### Experiment 4: Position Control of Mobile Robots
- **Topics Covered:** Theory of position control, moving to target points in a 2D plane, and trajectory tracking.
- **Focus:** Implementing a position controller (`traj_bot_control.cpp` in the `traj_control` package). The task includes calculating necessary translation and rotation, adjusting loop cycle times for smooth behavior, and tuning control parameters (Kw and Kv) for linear and angular velocities.
