/**
 * ==============================================================================
 * \file gradient_orientation_plot_node.cpp
 * \brief Publish representable values of the current AMiRo orientation and the
 *        gradient orientation.
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <std_msgs/Float32MultiArray.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <image_transport/subscriber_filter.h>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// Utils
#include "aal_potential_field_navigation/Constants.h"

// Types
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// process_v
/// \param msg
void process_v(const geometry_msgs::Twist::ConstPtr &msg, double &dir_d) {

    // get the motion command
    dir_d = msg->angular.z;

}

/// MAIN

/// main
/// \param argc_i
/// \param argv_pch
/// \return 0
int main(int argc_i, char *argv_pch[]) {

    // init ROS
    ros::init(argc_i, argv_pch, ros::this_node::getName());
    ros::NodeHandle node_c("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());
    ros::Rate loopRate_c(LOOP_RATE_i);

    GetPosition getPosition_c(ODOM_AMIRO_TOPIC_st);

    // prepare gradient listener
    double dir_d = 0.0;
    ros::Subscriber finalGradientSub_c = node_c.subscribe<geometry_msgs::Twist>(FINAL_GRADIENT_TOPIC_st, 1, boost::bind(process_v, _1, boost::ref(dir_d))); // subscribe to the final gradient topic

    // prepare orientation transmission
    ros::Publisher plotOrientationPub_c = node_c.advertise<std_msgs::Float32MultiArray>(PLOT_ORIENTATION_TOPIC_st, 1);
    std_msgs::Float32MultiArray arr_c;

    while (ros::ok()) {

        ros::spinOnce();

        arr_c.data.clear();
        arr_c.data.push_back(getPosition_c.getPositionByFrame_c(FRAME_MAP_st).theta);
        arr_c.data.push_back(dir_d);

        plotOrientationPub_c.publish(arr_c);

        loopRate_c.sleep();

    }

    ROS_INFO("Shutdown detected!");

}