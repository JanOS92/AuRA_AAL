/**
 * ==============================================================================
 * \file AvoidObstacle.cpp
 * \brief Provides the definition of the motor schema class AvoidObstacle
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/motor_schemas/AvoidObstacle.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

// Gazebo
//float alpha_f = 7.5f;
//float beta_f = 1.5f * alpha_f;
//float gamma_f = 0.05f;
//float overtakeAngle_f = PI_HALF_f * 0.20f;

// TWB
float alpha_f = 15.0f;
float beta_f = 1.5f * alpha_f;
float gamma_f = 0.05f;
//float overtakeAngle_f = PI_HALF_f * 0.20f;
float overtakeAngle_f = PI_HALF_f * 0.5f;

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ------------------------------------------------------
 * additional information are given in the AvoidObstacle.h
 * ------------------------------------------------------
*/
AvoidObstacle::AvoidObstacle(uint64_t id_ui) : MotorSchema(id_ui, MOTOR_SCHEMA_AVOID_OBSTACLE_NAME_str), getRingValues_c(PROXIMITY_RING_TOPIC_st),
                                               getPosition_c(ODOM_AMIRO_TOPIC_st) {

    this->pubMotionVec_c = this->nh_c.advertise<aal_potential_field_navigation::motionVectorMessage>(MOTION_VEC_TOPIC_st, 1); // advertise the general motion vector publishing topic

}

/**
 * Destructor
 * ------------------------------------------------------
 * additional information are given in the AvoidObstacle.h
 * ------------------------------------------------------
*/
AvoidObstacle::~AvoidObstacle() {

}

/**
 * Publish the motion commands
 * ------------------------------------------------------
 * additional information are given in the AvoidObstacle.h
 * ------------------------------------------------------
*/
bool AvoidObstacle::executeBehavior_b() {

    cv::Point2f vector_c = calcFObst_c();

//    ROS_INFO("vector_c.x = %f, vector_c.y = %f", vector_c.x, vector_c.y); // Debug only!

    this->publishMotionVector_v(vector_c, MotionVecMsgType ::COMMON, DEFAULT_MOTION_VEC_PRIO_i, this->name_s); // publish the motion vector

    return true;

}

/// PROTECTED METHODS

/// PRIVATE METHODS


/**
 * Calculate the resulting obstacle avoidance vector
 * ------------------------------------------------------
 * additional information are given in the AvoidObstacle.h
 * ------------------------------------------------------
*/
cv::Vec2f AvoidObstacle::calcFObst_c() {

    cv::Vec2d vResult_c(0.0, 0.0);

    // orientation in robot coordinates
    float theta_f = getPosition_c.getOrientation_f();
    cv::Vec2f v2R_c(cos(2.5f * overtakeAngle_f), sin(2.5f * overtakeAngle_f));
    cv::Vec2f v3R_c(cos(overtakeAngle_f), sin(overtakeAngle_f));
    cv::Vec2f v4R_c(cos(-overtakeAngle_f), sin(-overtakeAngle_f));
    cv::Vec2f v5R_c(cos(-2.5f * overtakeAngle_f), sin(-2.5f * overtakeAngle_f));

    // rotation
    float rotValues_af[4] = { cos(theta_f), -1.0f * sin(theta_f), sin(theta_f), cos(theta_f) };
    cv::Mat2f rotWR_c = cv::Mat(2, 2, CV_32F, rotValues_af);

    // base vector
    cv::Vec2f vF_c(gamma_f, 0.0);

    // sensor vectors
    cv::Vec2f v2_c(0.0, 0.0);
    cv::Vec2f v3_c(0.0, 0.0);
    cv::Vec2f v4_c(0.0, 0.0);
    cv::Vec2f v5_c(0.0, 0.0);

    // sensor vectors magnitude
    float v2_f = 0.0;
    float v3_f = 0.0;
    float v4_f = 0.0;
    float v5_f = 0.0;

    // sensor vectors magnitude calculation
    v2_f = beta_f * (float) (getRingValues_c.getPerceptualData_c(PROX_FRONT_RIGHT) - PROX_RING_SEN_VAL_OFF_ui) / (float) (MAX_PROX_RING_SEN_VAL_ui - PROX_RING_SEN_VAL_OFF_ui);
    v3_f = alpha_f * (float) (getRingValues_c.getPerceptualData_c(PROX_RIGHT_FRONT) - PROX_RING_SEN_VAL_OFF_ui) / (float) (MAX_PROX_RING_SEN_VAL_ui - PROX_RING_SEN_VAL_OFF_ui);
    v4_f = alpha_f * (float) (getRingValues_c.getPerceptualData_c(PROX_LEFT_FRONT) - PROX_RING_SEN_VAL_OFF_ui) / (float) (MAX_PROX_RING_SEN_VAL_ui - PROX_RING_SEN_VAL_OFF_ui);
    v5_f = beta_f * (float) (getRingValues_c.getPerceptualData_c(PROX_FRONT_LEFT) - getRingValues_c.getPerceptualData_c(PROX_FRONT_RIGHT)) / (float) (MAX_PROX_RING_SEN_VAL_ui); // preference overtake left


    // sensor vectors in robot coordinates
    v2_c = v2_f * v2R_c;
    v3_c = v3_f * v3R_c;
    v4_c = v4_f * v4R_c;
    v5_c = v5_f * v5R_c;

    // vectors in world coordinates
    vF_c = cv::Vec2f(rotWR_c.at<float>(0,0) * vF_c[0] + rotWR_c.at<float>(0,1) * vF_c[1], rotWR_c.at<float>(1,0) * vF_c[0] + rotWR_c.at<float>(1,1) * vF_c[1]);
    v2_c = cv::Vec2f(rotWR_c.at<float>(0,0) * v2_c[0] + rotWR_c.at<float>(0,1) * v2_c[1], rotWR_c.at<float>(1,0) * v2_c[0] + rotWR_c.at<float>(1,1) * v2_c[1]);
    v3_c = cv::Vec2f(rotWR_c.at<float>(0,0) * v3_c[0] + rotWR_c.at<float>(0,1) * v3_c[1], rotWR_c.at<float>(1,0) * v3_c[0] + rotWR_c.at<float>(1,1) * v3_c[1]);
    v4_c = cv::Vec2f(rotWR_c.at<float>(0,0) * v4_c[0] + rotWR_c.at<float>(0,1) * v4_c[1], rotWR_c.at<float>(1,0) * v4_c[0] + rotWR_c.at<float>(1,1) * v4_c[1]);
    v5_c = cv::Vec2f(rotWR_c.at<float>(0,0) * v5_c[0] + rotWR_c.at<float>(0,1) * v5_c[1], rotWR_c.at<float>(1,0) * v5_c[0] + rotWR_c.at<float>(1,1) * v5_c[1]);

    // vector merging
    vResult_c = vF_c + v2_c + v3_c + v4_c + v5_c;

    return vResult_c;

}