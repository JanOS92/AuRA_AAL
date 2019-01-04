/**
 * ==============================================================================
 * \file MotorSchema.cpp
 * \brief Provides the definition of the abstract motor schema class MotorSchema
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/motor_schemas/MotorSchema.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/// PROTECTED METHODS

/**
 * Publish the output vector (behavior()) to the move-
 * robot motor schema
 * ------------------------------------------------------
 * additional information are given in the MotorSchema.h
 * ------------------------------------------------------
*/
void MotorSchema::publishMotionVector_v(cv::Point2f vector_c, MotionVecMsgType type_e, int prio_i, std::string senderName_str) {

    aal_potential_field_navigation::motionVectorMessage msg_c;

    // prepare the motion message for publishing
    msg_c.sender_st = senderName_str;
    msg_c.type_i = type_e;
    msg_c.prio_i = prio_i;
    msg_c.x_f = vector_c.x;
    msg_c.y_f = vector_c.y;

    this->pubMotionVec_c.publish(msg_c); // publish motion vector message

}

/// PRIVATE METHODS
