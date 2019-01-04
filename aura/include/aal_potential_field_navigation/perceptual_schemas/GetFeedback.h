/**
 * ==============================================================================
 * \file GetFeedback.h
 * \brief Provides the declaration of the perceptual schema class GetFeedback
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_GETFEEDBACK_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_GETFEEDBACK_H

/// INCLUDES

// ROS
#include <sensor_msgs/Image.h>
#include "aal_potential_field_navigation/feedbackMessage.h"

// ROS-Service

// Types
#include "aal_potential_field_navigation/perceptual_schemas/PerceptualSchema.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class GetFeedback : public PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// GetFloorValues
    /// \param topic_st
    /// \brief Constructor
    GetFeedback(string topic_st, string nameMotorSchema_st);

    /// ~GetFloorValues
    /// \brief Destructor
    ~GetFeedback(); // Destructor

    /// getPerceptualData_f
    /// \return feedback value [0,1]
    /// \brief Return the floor sensor value
    float getPerceptualData_f();

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// coopWeightCallback_v
    /// \param msg_c
    /// \brief Callback of the feedback_c subscriber
    void feedbackCallback_v(const aal_potential_field_navigation::feedbackMessage::ConstPtr& msg_c);

    /// LOCAL ATTRIBUTES

    ros::Subscriber subFeedback_c; // feedback subscriber
    string name_st; // name of the according motor schema
    float feedbackWeight_f = 1.0;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_GETFEEDBACK_H
