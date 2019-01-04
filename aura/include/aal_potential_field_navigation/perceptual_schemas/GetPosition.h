/**
 * ==============================================================================
 * \file  GetPosition.h
 * \brief Provides the declaration of the perceptual schema class GetPostion
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_GETPOSITION_HPP
#define AAL_POTENTIAL_FIELD_NAVIGATION_GETPOSITION_HPP

/// INCLUDES

// ROS
#include <ros/ros.h>
#include "nav_msgs/Odometry.h"
#include <tf/transform_listener.h>

// Types
#include "PerceptualSchema.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class GetPosition : public PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// GetPosition
    /// \param topic_st
    /// \param baseFrame_st
    /// \brief Constructor
    GetPosition(string topic_st, string baseFrame_st = FRAME_AMIRO_BASE_LINK_st);

    /// ~GetPosition
    /// \brief Destructor
    ~GetPosition();

    /// getPosition_c
    /// \return [0]: x, [1]: y
    /// \brief Return the current position in the world frame
    std::pair<float,float> getPosition_c();

    /// getPositionByFrame_c
    /// \param frameId_st
    /// \return Pose(x,y,z)
    /// \brief Return the current position in the given frame
    Pose getPositionByFrame_c(string frameId_st);

    /// getOrientation_f
    /// \return eulYaw_f
    /// \brief Return the current orientation
    float getOrientation_f();

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    void odometryCallback_v(const nav_msgs::Odometry::ConstPtr& msg);

    /// LOCAL ATTRIBUTES

    ros::Subscriber subOdometry_c; // RFID-Tag subscriber
    tf::TransformListener listener_c;

    float posX_f;
    float posY_f;
    float eulYaw_f;

    string baseFrame_st;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_GETPOSITION_HPP
