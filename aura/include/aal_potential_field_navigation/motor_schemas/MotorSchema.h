/**
 * ==============================================================================
 * \file MotorSchema.h
 * \brief Provides the declaration of the abstract class MotorSchema
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_MOTORSCHEMA_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_MOTORSCHEMA_H

/// INCLUDES

// ROS
#include "aal_potential_field_navigation/motionVectorMessage.h"

// Utils
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/utils/utils.h"
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/utils/potential_field_utils.hpp"

// Types
#include <list>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class MotorSchema {

public:

    /// GLOBAL METHODS

    /// MotorSchema
    /// \param id_ui_
    /// \brief Constructor
    MotorSchema(uint64_t id_ui, string name_s) : id_ui(id_ui), name_s(name_s) {};

    /// ~MotorSchema
    /// \brief Destructor
    virtual ~MotorSchema() {};

    /// getId_ui
    /// \return motor schema id
    /// \brief Return the id of the motor schema instance
    virtual uint64_t getId_ui() { return this->id_ui; };

    /// executeBehavior_b
    /// \return true if succeed else false
    /// \brief Determine the motion command and transmit them to the move-robot schema
    virtual bool executeBehavior_b() = 0;

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// publishMotionVector_v
    /// \param vector_c
    /// \param type_e
    /// \param prio_i
    /// \param senderName_str
    /// \brief Publish the specific motor commands
    virtual void publishMotionVector_v(cv::Point2f vector_c, MotionVecMsgType type_e, int prio_i, std::string senderName_str);

    /// LOCAL/INHERTIABLE ATTRIBUTES

    ros::NodeHandle nh_c;

    ros::Publisher pubMotionVec_c;

    ros::Subscriber subCoopWeight_c;

    std::list<ErrorCode> errorLog_lst; // ErrorCode buffer

    uint64_t id_ui;

    string name_s; // motor schema dedicated name

private:

    /// LOCAL METHODS

    /// LOCAL ATTRIBUTES

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_MOTORSCHEMA_H