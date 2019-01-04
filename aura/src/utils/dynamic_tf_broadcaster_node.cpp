/// ==============================================================================
/// @file dynamic_tf_broadcaster.cpp
/// @brief Provides the necessary tf for the TWB implementation
/// @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
/// @date 11.07.2018
/// ==============================================================================

/// INCLUDES

// ROS
#include "ros/ros.h"
#include "tf/transform_broadcaster.h"

// Types
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"

// Utils
#include "aal_potential_field_navigation/Constants.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

// Pose
double mapOriginX_d = 0.0;
double mapOriginY_d = 0.0;
double mapOriginZ_d = 3.7; // [m], TWB camera height
tf::Quaternion mapOriginTheta_c;

// tf
double roll_d = (double)PI_f;

/// LOCAL FUNCTIONS

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

//    GetPosition getPosition_c(ODOM_MARKER_TOPIC_st, FRAME_MARKER_st); // perceptual schema
    geometry_msgs::Pose odomPose_tf;

    tf::TransformBroadcaster br_c;
    tf::TransformListener listener_c;
    tf::StampedTransform transform_c;

//    while (node_c.ok()) {
//
////        ros::spinOnce();
//
//        Pose pose_str = getPosition_c.getPositionByFrame_c(FRAME_ID_CAMERA_st); // get the pose of the robot with reference to the camera frame
//
//        mapOriginX_d = pose_str.x; // get the x coordinate of the map
//        mapOriginY_d = pose_str.y; // get the y coordinate of the map
//        mapOriginZ_d = pose_str.z; // get the z coordinate of the map
//        mapOriginTheta_d = pose_str.theta; // get the orientation of the map
//
////        ROS_INFO("x = %f, y = %f, theta = %f", pose_str.x, pose_str.y, pose_str.theta); // Debug only!
//
//        transform_c.setOrigin( tf::Vector3(mapOriginX_d, mapOriginY_d, mapOriginZ_d) ); // set the origin (mind that the translation will be applied before the rotation : -1.0 * x)
//
//        tf::Quaternion q_c = tf::createQuaternionFromRPY(roll_d, 0.0, mapOriginTheta_d); // convert the roll and yaw angle (theta) to a quaternion
//
//        transform_c.setRotation( q_c ) ; // set the rotation
//
//        br_c.sendTransform( tf::StampedTransform(transform_c, ros::Time::now(), FRAME_ID_CAMERA_st, FRAME_ID_MAP_st) );
//
//        loopRate_c.sleep();
//
//    }

    while(node_c.ok()) {

        try {

            listener_c.waitForTransform(FRAME_MAP_st, FRAME_CAMERA_st, ros::Time(0),
                                               TIME_TO_WAIT_OF_STP_c); // wait for transformation

            listener_c.lookupTransform(FRAME_MAP_st, FRAME_CAMERA_st, ros::Time(0),
                                              transform_c); // apply transformation between the tfs

        } catch (tf::TransformException ex) {

            ROS_ERROR("[%s] %s", ros::this_node::getName().c_str(), ex.what());

        }

        mapOriginX_d = transform_c.getOrigin().x() + (IMAGE_WIDTH_i/2.0f) * METER_PER_PIXEL_WIDTH_d; // get the new x coordinate of the origin w.r.t. "FRAME_MARKER_st"
        mapOriginY_d = transform_c.getOrigin().y() + (IMAGE_HEIGHT_i/2.0f) * METER_PER_PIXEL_HEIGHT_d; // get the new y coordinate of the origin w.r.t. "FRAME_MARKER_st"
        mapOriginZ_d = 0.0; // set the new z coordinate of the origin w.r.t. "FRAME_MARKER_st"

        transform_c.setOrigin( tf::Vector3(mapOriginX_d, mapOriginY_d, mapOriginZ_d) ); // set the origin (mind that the translation will be applied before the rotation : -1.0 * x)

        mapOriginTheta_c = transform_c.getRotation(); // get the rotation of the "FRAME_MARKER_st" w.r.t. "FRAME_CAMERA_st"

        tf::Quaternion q_c = tf::createQuaternionFromRPY(roll_d, 0.0, 0.0) * mapOriginTheta_c; // convert the roll and yaw angle (theta) to a quaternion

        transform_c.setRotation( q_c ) ; // set the rotation

        br_c.sendTransform( tf::StampedTransform(transform_c, ros::Time::now(), FRAME_MAP_st, FRAME_POT_FIELD_st) );

        loopRate_c.sleep();

    }

    return 0;

}

