/**
 * ==============================================================================
 * \file WaitForObstacle.cpp
 * \brief Provides the definition of the motor schema class WaitForObstacle
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/motor_schemas/WaitForObstacle.h"

/// NAMESPACES

using namespace std;

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ------------------------------------------------------
 * additional information are given in the WaitForObstacle.h
 * ------------------------------------------------------
*/
WaitForObstacle::WaitForObstacle(uint64_t id_ui) : MotorSchema(id_ui, WAIT_FOR_OBSTACLE_st), getRingValues_c(PROXIMITY_RING_TOPIC_st) {

    this->pubMotionVec_c = this->nh_c.advertise<aal_potential_field_navigation::motionVectorMessage>(MOTION_VEC_TOPIC_st, 1); // advertise the general motion vector publishing topic

}

/**
 * Destructor
 * ------------------------------------------------------
 * additional information are given in the WaitForObstacle.h
 * ------------------------------------------------------
*/
WaitForObstacle::~WaitForObstacle() {

}

/**
 * Publish the motion commands
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
bool WaitForObstacle::executeBehavior_b() {

    uint16_t distanceValueFrontLeft_ui = this->getRingValues_c.getPerceptualData_c(PROX_FRONT_LEFT); // get the value of the front left ring sensor
    uint16_t distanceValueFrontRight_ui = this->getRingValues_c.getPerceptualData_c(PROX_FRONT_RIGHT); // get the value of the front right ring sensor

    cv::Point2f vector_c(0.0, 0.0);

//    ROS_INFO("minDistance_d = %f", minDistance_d); // Debug only!

    if (distanceValueFrontLeft_ui >= MINIMUM_DISTANCE_ui || distanceValueFrontRight_ui >= MINIMUM_DISTANCE_ui) { // check if the minimum distance is overshoot, if so change the message type to "INTERRUPT"

        this->publishMotionVector_v(vector_c, MotionVecMsgType ::INTERRUPT, DEFAULT_MOTION_VEC_PRIO_i, this->name_s); // publish the motion vector

    }

    return true;

}

/// PROTECTED METHODS

/// PRIVATE METHODS