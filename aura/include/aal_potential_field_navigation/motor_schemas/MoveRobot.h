/**
 * ==============================================================================
 * \file MoveRobot.h
 * \brief Provides the declaration of the motor schema class MoveRobot
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_MOVEROBOT_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_MOVEROBOT_H

/// INCLUDES

// ROS
#include <nav_msgs/Odometry.h>

// Types
#include "MotorSchema.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetMotionVector.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"

// Boost
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

/// NAMESPACES

using namespace std;

using namespace boost::accumulators;

/// CLASS DECLARATION

class MoveRobot : public MotorSchema {

public:

    /// GLOBAL METHODS

    /// MoveRobot
    /// \param id_ui
    /// \brief Constructor
    MoveRobot(uint64_t id_ui);

    /// ~MoveRobot
    /// \brief Destructor
    ~MoveRobot();

    /// executeBehavior_b
    /// \return true if succeed else false
    /// \brief Determine the kinematic command and transmit them to the move-robot schema
    bool executeBehavior_b();

    /// stopRobot_v
    /// \brief Stop all motor actions
    void stopRobot_v();

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// publishMotionVector_v
    /// \param motionVec_s
    /// \brief Publish the specific motor commands
    void publishMotionVector_v(MotionVector motionVec_s);

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// listen_v
    /// \param motionVec_s
    /// \brief Manage the motion vectors on the associated topic
    void listen_v(MotionVector motionVec_s);

    /// LOCAL ATTRIBUTES

    std::map<std::string, MotionVector> motionVec_mp;

    MotionVector finalVec_s;

    bool isStartTimeSet;

    ros::Time startTime;

    ros::Duration timer_c;

    ros::Publisher pubFinalGradient_c;

    /// PERCEPTUAL SCHEMAS

    GetMotionVector getMotionVector_c;
    GetPosition getPosition_c;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_MOVEROBOT_H
