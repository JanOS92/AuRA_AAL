/**
 * ==============================================================================
 * \file GetRingValues.h
 * \brief Provides the declaration of the perceptual schema class GetRingValues
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_GETRINGVALUES_HPP
#define AAL_POTENTIAL_FIELD_NAVIGATION_GETRINGVALUES_HPP

/// INCLUDES

// Types
#include "aal_potential_field_navigation/perceptual_schemas/PerceptualSchema.h"

// ROS
#include <ros/ros.h>
#if TWB == 1
#include <sai_msgs/Int32MultiArrayStamped.h>
#elif TWB == 0
#include <amiro_msgs/UInt16MultiArrayStamped.h>
#endif

// Utils
#include "aal_potential_field_navigation/utils/image_converting_utils.h"
#include "aal_potential_field_navigation/utils/utils.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class GetRingValues : public PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// GetMinDistance
    /// \param topic_st
    /// \brief Constructor
    GetRingValues(string topic_st);

    /// ~GetMinDistance
    /// \brief Destructor
    ~GetRingValues();

    /// getPerceptualData_mat
    /// \return sensor values
    /// \brief Return the ring sensor value
    uint16_t getPerceptualData_c(RingSensor ringSensor_e);

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// ProximityCallback_v
    /// \param msg_c
    /// \brief Callback of the subRfidTagList_c subscriber
#if TWB == 1
    void proximityRingCallback_v(const sai_msgs::Int32MultiArrayStamped::ConstPtr& msg_c); // RFID-Tag callback
#elif TWB == 0
    void proximityRingCallback_v(const amiro_msgs::UInt16MultiArrayStamped::ConstPtr& msg_c); // RFID-Tag callback
#endif

    /// LOCAL ATTRIBUTES

    uint16_t proximityRingValues_aui[PROXIMITY_SENSORS_CNT_i];
    ros::Subscriber subProximityRing_c; // Proximity-Ring subscriber

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_GETRINGVALUES_HPP
