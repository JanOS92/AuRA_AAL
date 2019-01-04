/**
 * ==============================================================================
 * \file  GetFloorValues.h
 * \brief Provides the declaration of the perceptual schema class GetFloorValues
 * \author Jan O'Sullivan <josullivan\techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_GETFLOORVALUES_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_GETFLOORVALUES_H

/// INCLUDES

// Types
#include "PerceptualSchema.h"

// ROS
#include <ros/ros.h>
#if TWB == 1
#include <sai_msgs/Int32MultiArrayStamped.h>
#elif TWB == 0
#include <amiro_msgs/UInt16MultiArrayStamped.h>
#endif

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class GetFloorValues : public PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// GetFloorValues
    /// \param topic_st
    /// \brief Constructor
    GetFloorValues(string topic_st);

    /// ~GetFloorValues
    /// \brief Destructor
    ~GetFloorValues(); // Destructor

    /// getPerceptualData_ui
    /// \param floorSensor_e
    /// \return floor sensor value
    /// \brief Return the floor sensor value
    uint16_t getPerceptualData_ui(FloorSensor floorSensor_e);

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// FloorSensorCallback_v
    /// \param msg_c
    /// \brief Callback of the subFloorSensorValues_c subscriber
#if TWB == 1
    void floorSensorCallback_v(const sai_msgs::Int32MultiArrayStamped::ConstPtr &msg); // Floor sensor callback
#elif TWB == 0
    void floorSensorCallback_v(const amiro_msgs::UInt16MultiArrayStamped::ConstPtr &msg); // Floor sensor callback
#endif

    /// LOCAL ATTRIBUTES

    uint16_t floorSensorValues_aui[FLOOR_SENSORS_CNT_i];
    ros::Subscriber subFloorSensorValues_c; // Floor sensor subscriber

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_GETFLOORVALUES_H
