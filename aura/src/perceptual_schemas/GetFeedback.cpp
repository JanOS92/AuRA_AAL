/**
 * ==============================================================================
 * \file  GetFeedback.cpp
 * \brief Provides the definition of the perceptual schema class GetFeedback
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/perceptual_schemas/GetFeedback.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ------------------------------------------------------
 * additional information are given in the GetFeedback.h
 * ------------------------------------------------------
*/
GetFeedback::GetFeedback(string topic_st, string nameMotorSchema_st) : PerceptualSchema(topic_st), name_st(nameMotorSchema_st) {

}

/**
 * Destructor
 * ------------------------------------------------------
 * additional information are given in the GetFeedback.h
 * ------------------------------------------------------
*/
GetFeedback::~GetFeedback() {

}

/**
 * Return the specific perceptual data
 * ------------------------------------------------------
 * additional information are given in the GetFeedback.h
 * ------------------------------------------------------
*/
float GetFeedback::getPerceptualData_f() {

    return this->feedbackWeight_f;

}

/// PROTECTED METHODS

/// PRIVATE METHODS

/**
 * Listen to the feedback topic and check if there is
 * a feedbackWeight_f which is dedicated to the underlying
 * MotorSchema
 * ------------------------------------------------------
 * additional information are given in the GetFeedback.h
 * ------------------------------------------------------
*/
void GetFeedback::feedbackCallback_v(const aal_potential_field_navigation::feedbackMessage::ConstPtr& msg_c) {

    if ((0 <= msg_c->weight_f <= 1) && msg_c->recipient_st == this->name_st) { // only accept the feedbackWeight_f if the recipient (name) matches and the value is in range

        this->feedbackWeight_f = msg_c->weight_f;

    } else {

        this->feedbackWeight_f = 1.0f;

    }

}