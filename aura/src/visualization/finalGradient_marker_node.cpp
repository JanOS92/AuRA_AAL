/**
 * ==============================================================================
 * \file finalGradient_marker_c_node.cpp
 * \brief Receive the merged "motionVector" and send it as marker_c.
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <visualization_msgs/Marker.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_broadcaster.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <image_transport/subscriber_filter.h>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// Types and classes
#include "aal_potential_field_navigation/perceptual_schemas/GetMotionVector.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"
#include "aal_potential_field_navigation/Types.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

ros::Publisher markerPub_c;
uint32_t shape = visualization_msgs::Marker::ARROW;
double mapOriginZ_d = 3.7; // [m], TWB camera height

/// LOCAL FUNCTIONS

/// process_v
/// \param msg
void process_v(const geometry_msgs::Twist::ConstPtr &msg, GetPosition &getPosition_c) {

    // get the position of the AMiRo in some frame
    Pose poseFrame_c = getPosition_c.getPositionByFrame_c(FRAME_MAP_st);
//    Pose poseCam_c = getPosition_c.getPositionByFrame_c(FRAME_CAMERA_st);

    // get the motion command
    double mag_d = msg->linear.x;
    double dir_d = msg->angular.z;

    visualization_msgs::Marker marker_c;

    // set the frame ID and timestamp
    marker_c.header.frame_id = FRAME_MAP_st;
    marker_c.header.stamp = ros::Time::now();

    // set the namespace and id for this marker_c
    marker_c.ns = "motionVector";
    marker_c.id = 0;

    // set the marker_c type
    marker_c.type = shape;

    // set the position of the marker_c to the origin of the according frame
    marker_c.pose.position.x = poseFrame_c.x;
    marker_c.pose.position.y = poseFrame_c.y;
    marker_c.pose.position.z = poseFrame_c.z;

    // set the orientation of the marker_c
    tf::Quaternion q_c = tf::createQuaternionFromRPY(0.0, 0.0, dir_d);
    marker_c.pose.orientation.x = q_c.x();
    marker_c.pose.orientation.y = q_c.y();
    marker_c.pose.orientation.z = q_c.z();
    marker_c.pose.orientation.w = q_c.w();

    // set the scale of the marker_c -- 1 x 1 x 1 here means 1m on a side
    marker_c.scale.x = 0.1 * mag_d/MAX_MOTION_VEC_ABS_d;
    marker_c.scale.y = 0.01;
    marker_c.scale.z = 0.01;

    // set our initial shape type to be a cube
    uint32_t shape = visualization_msgs::Marker::ARROW;

    // set the color -- be sure to set alpha to something non-zero!
    marker_c.color.r = 0.0f;
    marker_c.color.g = 1.0f;
    marker_c.color.b = 0.0f;
    marker_c.color.a = 1.0;

    markerPub_c.publish(marker_c);

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

    // Subscriber
    ros::Subscriber sub_c = node_c.subscribe<geometry_msgs::Twist>(FINAL_GRADIENT_TOPIC_st, 1,
                                                                   boost::bind(process_v, _1, boost::ref(getPosition_c))); // subscribe to the final gradient topic

    // Publisher
    markerPub_c = node_c.advertise<visualization_msgs::Marker>(FINAL_GRADIENT_MARKER_TOPIC_st, 1);

    ros::spin(); // keep this node alive

}
