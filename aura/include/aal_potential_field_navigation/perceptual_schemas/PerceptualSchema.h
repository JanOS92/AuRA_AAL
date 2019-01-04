/**
 * ==============================================================================
 * \file PerceptualSchema.h
 * \brief Provides the definition of the abstract class PerceptualSchema
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_PERCEPTUALSCHEMA_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_PERCEPTUALSCHEMA_H

/// INCLUDES

// ROS
#include <ros/ros.h>
#include "aal_potential_field_navigation/motionVectorMessage.h"

// Utils
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/utils/utils.h"
#include "aal_potential_field_navigation/Constants.h"

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

// Utils
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/utils/utils.h"
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/navigation/navigation_utils.h"
#include <utility>

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// PerceptualSchema
    /// \param topic_st_
    /// \brief Constructor
    PerceptualSchema(string topic_st_) : topic_st(topic_st_) {};

    /// ~PerceptualSchema
    /// \brief Destructor
    virtual ~PerceptualSchema() {};

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

    string topic_st;

    ros::NodeHandle nh_c; // Node Handle

    std::list<ErrorCode> errorLog_lst; // Error buffer

private:

    /// LOCAL METHODS

    /// LOCAL ATTRIBUTES

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_PERCEPTUALSCHEMA_H
