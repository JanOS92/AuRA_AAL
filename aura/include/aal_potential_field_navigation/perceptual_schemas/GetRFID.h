/**
 * ==============================================================================
 * \file  GetRFID.h
 * \brief Provides the declaration of the perceptual schema class GetRFID
 * \author Jan O'Sullivan <josullivan\techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_IDENTIFYRFIDTAG_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_IDENTIFYRFIDTAG_H

/// INCLUDES

// ROS
#include <ros/ros.h>
#include "std_msgs/UInt32MultiArray.h"

// ROS-Service
#include "../../../../install/include/aal_potential_field_navigation/get_rfid_type.h"

// Types
#include "PerceptualSchema.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class GetRFID : public PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// GetRFID
    /// \param topic_st
    /// \brief Constructor
    GetRFID(string topic_st);

    /// ~GetRFID
    /// \brief Destructor
    ~GetRFID(); // Destructor

    /// getInitialRFID_ui
    /// \return initial RFID
    /// \brief Return the first registered RFID
    uint64_t getInitialRFID_ui();

    /// getPerceptualData_pair
    /// \return [0]: lastRFID,[1]: currentRFID
    /// \brief Return the specific perceptual data
    std::pair<uint64_t, uint64_t> getPerceptualData_pair();

    /// getCurrentRFIDType
    /// \return RFID tag type
    /// \brief Return the type of the current RFID tag
    RFIDType getCurrentRFIDType_e();

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// RFIDCallback_v
    /// \param msg_c
    /// \brief Callback of the subRfidTagList_c subscriber
    void RFIDCallback_v(const std_msgs::UInt32MultiArray::ConstPtr& msg_c); // RFID-Tag callback

    /// LOCAL ATTRIBUTES

    uint64_t currentRFID_ui;
    uint64_t lastRFID_ui;
    uint64_t initialRFID_ui;
    uint64_t currentRFIDBuffer_ui;
    ros::ServiceClient getRFIDTypeServiceClient_c;
    ros::Subscriber subRfidTagList_c; // RFID-Tag subscriber

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_IDENTIFYRFIDTAG_H
