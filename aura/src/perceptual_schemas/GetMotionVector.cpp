/**
 * ==============================================================================
 * \file  GetMotionVector.cpp
 * \brief Provides the definition of the perceptual schema class GetMotionVector
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/perceptual_schemas/GetMotionVector.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ---------------------------------------------------------
 * additional information are given in the GetMotionVector.h
 * ---------------------------------------------------------
*/
GetMotionVector::GetMotionVector(string topic_st) : PerceptualSchema(topic_st),
                                                          motionVec_s(DEFAULT_MOTION_VEC_s) {

    this->subMotionVec_c = this->nh_c.subscribe<aal_potential_field_navigation::motionVectorMessage>(this->topic_st, 1, &GetMotionVector::motionVectorCallback_v,
                                                                                                     this); // set up the subscriber and bind the "motionVectorCallback_v" to the "topic"

}

/**
 * Destructor
 * ---------------------------------------------------------
 * additional information are given in the GetMotionVector.h
 * ---------------------------------------------------------
*/
GetMotionVector::~GetMotionVector() {

}

/**
 * Return the specific perceptual data
 * ---------------------------------------------------------
 * additional information are given in the GetMotionVector.h
 * ---------------------------------------------------------
*/
MotionVector GetMotionVector::getPerceptualData_s() {

    return this->motionVec_s; // return the current value

}

/// PROTECTED METHODS

/// PRIVATE METHODS

/**
 * Callback function which is dedicated to the perceptual
 * schema specific topic
 * ---------------------------------------------------------
 * additional information are given in the GetMotionVector.h
 * ---------------------------------------------------------
*/
void GetMotionVector::motionVectorCallback_v(const aal_potential_field_navigation::motionVectorMessage::ConstPtr& msg_c) {

    if (!msg_c->sender_st.empty()) { // check for incorrect messages

        this->motionVec_s = { msg_c->x_f, msg_c->y_f, (MotionVecMsgType)msg_c->type_i, msg_c->prio_i, msg_c->sender_st };

    } else { // else set default

        this->motionVec_s = DEFAULT_MOTION_VEC_s;

    }

}
