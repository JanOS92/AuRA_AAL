/**
 * ==============================================================================
 * \file GetDirection.h
 * \brief Provides the declaration of the perceptual schema class GetDirection
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_GETLINEFOLLOWINGCOMMAND_HPP
#define AAL_POTENTIAL_FIELD_NAVIGATION_GETLINEFOLLOWINGCOMMAND_HPP

/// INCLUDES

// ROS
#include <sensor_msgs/Image.h>

// ROS-Service
#include "../../../../install/include/aal_potential_field_navigation/get_field_by_key.h"
#include "../../../../install/include/aal_potential_field_navigation/check_rfid.h"

// Types
#include "aal_potential_field_navigation/perceptual_schemas/PerceptualSchema.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class GetDirection : public PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// GetDirection
    /// \brief Constructor
    GetDirection();

    /// ~GetDirection
    /// \brief Destructor
    ~GetDirection();

    /// getPerceptualData_mat
    /// \param vectorfield
    /// \param targetRFID_ui
    /// \param currentRFID_ui
    /// \param lastRFID_ui
    /// \param initialRFID_ui
    /// \return true if succeed else false
    /// \brief Get the necessary vectorfield as result of the navigation concept
    bool
    getPerceptualData_b(cv::Mat &vectorfield, uint64_t targetRFID_ui, uint64_t currentRFID_ui, uint64_t lastRFID_ui,
                        uint64_t initialRFID_ui);

    /// checkRFID_b
    /// \param srv_c
    /// \param currentRFID_ui
    /// \param lastRFID_ui
    /// \return true if succeed else false
    /// \brief Call the checkRFID service and write the outcome into the "srv_c" variable
    bool checkRFID_b(aal_potential_field_navigation::check_rfid &srv_c, uint64_t currentRFID_ui, uint64_t lastRFID_ui);

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// generateFieldKey_b
    /// \param fieldKey
    /// \param targetRFID_ui
    /// \param currentRFID_ui
    /// \param initialRFID_ui
    /// \param lastRFID_ui
    /// \return true if succeed else false
    /// \brief Generate a field key string as part of the navigation concept
    bool generateFieldKey_b(string &fieldKey, uint64_t targetRFID_ui, uint64_t currentRFID_ui, uint64_t initialRFID_ui,
                            uint64_t lastRFID_ui);

    /// LOCAL ATTRIBUTES

    cv::Mat data_mat; // data
    ros::ServiceClient getFieldByKeyServiceClient_c;
    ros::ServiceClient checkRFIDServiceClient_c;
    uint64_t targetRFID_ui;
    uint64_t initialRFID_ui;
    string fieldKeyBuffer_st;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_GETLINEFOLLOWINGCOMMAND_HPP
