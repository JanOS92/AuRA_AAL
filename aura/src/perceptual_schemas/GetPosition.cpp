/**
 * ==============================================================================
 * @file  GetPosition.cpp
 * @brief Provides the definition of the perceptual schema class GetPostion
 * @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ---------------------------------------------------------
 * additional information are given in the GetPosition.h
 * ---------------------------------------------------------
*/
GetPosition::GetPosition(string topic_st, string baseFrame_st) : PerceptualSchema(topic_st), posY_f(DEFAULT_POS_f),
                                                                 posX_f(DEFAULT_POS_f), eulYaw_f(DEFAULT_EUL_YAW_f),
                                                                 baseFrame_st(baseFrame_st) {

    this->subOdometry_c = this->nh_c.subscribe<nav_msgs::Odometry>(this->topic_st, 1, &GetPosition::odometryCallback_v,
                                                                   this); // set up the subscriber and bind the "odometryCallback" to the "topic"

}

/**
 * Destructor
 * ---------------------------------------------------------
 * additional information are given in the GetPosition.h
 * ---------------------------------------------------------
*/
GetPosition::~GetPosition() {

}

/**
 * getPosition_c
 * ---------------------------------------------------------
 * additional information are given in the GetPosition.h
 * ---------------------------------------------------------
*/
std::pair<float, float> GetPosition::getPosition_c() {

    return std::make_pair(this->posX_f, this->posY_f);

}

/**
 * getOrientation_f
 * ---------------------------------------------------------
 * additional information are given in the GetPosition.h
 * ---------------------------------------------------------
*/
float GetPosition::getOrientation_f() {

    return this->eulYaw_f;

}

/**
 * getPositionByFrame_c
 * ---------------------------------------------------------
 * additional information are given in the GetPosition.h
 * ---------------------------------------------------------
*/
Pose GetPosition::getPositionByFrame_c(string frameId_st) {

    Pose pose_c = DEFAULT_POSE_c;

    tf::StampedTransform transform_c; // transformation between the robot and the vector field

    try {

        this->listener_c.waitForTransform(frameId_st, this->baseFrame_st, ros::Time(0),
                                          TIME_TO_WAIT_OF_STP_c); // wait for transformation

        this->listener_c.lookupTransform(frameId_st, this->baseFrame_st, ros::Time(0),
                                         transform_c); // apply transformation between the tfs

    } catch (tf::TransformException ex) {

        ROS_ERROR("[%s] %s", ros::this_node::getName().c_str(), ex.what());

        return DEFAULT_POSE_c; // if an error occurs, return the default value

    }

    pose_c.x = transform_c.getOrigin().x(); // get the new x coordinate inside the given frame
    pose_c.y = transform_c.getOrigin().y(); // get the new y coordinate inside the given frame
    pose_c.z = transform_c.getOrigin().z(); // get the new z coordinate inside the given frame
    pose_c.theta = tf::getYaw(transform_c.getRotation()); // get the new rotation inside the given frame

    return pose_c;

}

/// PROTECTED METHODS

/// PRIVATE METHODS


/**
 * Callback function which is dedicated to the perceptual
 * schema specific topic
 * ---------------------------------------------------------
 * additional information are given in the GetPosition.h
 * ---------------------------------------------------------
*/
void GetPosition::odometryCallback_v(const nav_msgs::Odometry::ConstPtr &msg) {

    tf::Pose pose_c;

    this->posX_f = msg->pose.pose.position.x; // get the x position

    this->posY_f = msg->pose.pose.position.y; // get the y position

    tf::poseMsgToTF(msg->pose.pose, pose_c); // get the pose

    this->eulYaw_f = tf::getYaw(pose_c.getRotation()); // get the orientation

}