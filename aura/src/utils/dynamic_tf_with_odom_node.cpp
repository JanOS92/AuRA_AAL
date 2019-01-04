/// ============================================================================
/// Name        : dynamic_tf_with_odom.cpp
/// Author      : Daniel Rudolph <drudolph@techfak.uni-bielefeld.de>, Timo Korthals <tkorthals@techfak.uni-bielefeld.de>
/// Description : Recieve ros::nav_mags::Odometry and dynamic tf then with given frame_id
/// Source      : https://raw.githubusercontent.com/tik0/amiro_slam/master/src/dynamic_tf_with_odom.cpp
/// ============================================================================

/// INCLUDES

// ROS
#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>

// Utils
#include "aal_potential_field_navigation/Constants.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

// parameter
string rosListenerTopic_str;
string parentFrame_str;
string childFrame_str;
float zOffset_f;
geometry_msgs::Pose lastOdomPose_c;
float alphaPosition_f;
bool applyLowPass_b;

/// LOCAL FUNCTIONS

/// lowpass_c
/// \param odomPose_c
/// \param lastOdomPose_c
/// \param alphaPosition_f
/// \param alphaOrientation_f
/// \brief y[i] := α * x[i] + (1-α) * y[i-1]
void lowpass_c(geometry_msgs::Pose &odomPose_c, geometry_msgs::Pose &lastOdomPose_c, float alphaPosition_f) {

    // position
    odomPose_c.position.x = alphaPosition_f * odomPose_c.position.x + (1.0f - alphaPosition_f) * lastOdomPose_c.position.x;
    odomPose_c.position.y = alphaPosition_f * odomPose_c.position.y + (1.0f - alphaPosition_f) * lastOdomPose_c.position.y;
    odomPose_c.position.z = alphaPosition_f * odomPose_c.position.z + (1.0f - alphaPosition_f) * lastOdomPose_c.position.z;

    lastOdomPose_c = odomPose_c;

}

/// process_v
/// \param msg
void process_v(const nav_msgs::Odometry::ConstPtr &msg) {

    static tf::TransformBroadcaster br;

    tf::Transform transform_c;

    if (applyLowPass_b) {

        geometry_msgs::Pose odomPose_c = msg->pose.pose;

        lowpass_c(odomPose_c, lastOdomPose_c, alphaPosition_f); // apply a low pass filter on the pose

        transform_c.setOrigin(tf::Vector3(odomPose_c.position.x, odomPose_c.position.y, zOffset_f));


        transform_c.setRotation(
                tf::Quaternion(odomPose_c.orientation.x, odomPose_c.orientation.y, odomPose_c.orientation.z,
                               odomPose_c.orientation.w));


    } else {

        transform_c.setOrigin(
                tf::Vector3(msg->pose.pose.position.x, msg->pose.pose.position.y, msg->pose.pose.position.z));

        transform_c.setRotation(
                tf::Quaternion(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z,
                               msg->pose.pose.orientation.w));

    }

    br.sendTransform(
            tf::StampedTransform(transform_c, msg->header.stamp,
                                 parentFrame_str.empty() ? msg->child_frame_id : parentFrame_str,
                                 childFrame_str)); // broadcast a tf with the child frame "FRAME_MARKER_st" which is located at the same position as the marker

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
    ros::Rate loopRate_(LOOP_RATE_i);

    // parameter
    node_c.param<string>("ros_listener_odom_topic", rosListenerTopic_str, "/topic");
    node_c.param<string>("child_frame", childFrame_str, "");
    node_c.param<string>("parent_frame", parentFrame_str, "");
    node_c.param<float>("z_offset", zOffset_f, 0.0);
    node_c.param<float>("alpha_position", alphaPosition_f, 1.0);
    node_c.param<bool>("apply_low_pass", applyLowPass_b, false);

//    ROS_INFO("%s, %s", rosListenerTopic_str.c_str(), childFrame_str.c_str()); // Debug only!

    ros::Subscriber sub_c = node_c.subscribe(rosListenerTopic_str, 1, process_v); // subscribe to the map marker

    ros::spin(); // keep this node alive

    return 0;

}