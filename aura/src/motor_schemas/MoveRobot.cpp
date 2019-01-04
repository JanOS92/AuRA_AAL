/**
 * ==============================================================================
 * \file MoveRobot.cpp
 * \brief Provides the definition of the motor schema class MoveRobot
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/motor_schemas/MoveRobot.h"

/// NAMESPACES

using namespace std;

using namespace boost::accumulators;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// checkSender_b
/// \param sender_s
/// \return true if the sender is listed, else false
/// \brief Check the sender on the basis of the sender list
bool checkSender_b(std::string sender_s) {

    for (auto it = MOTOR_SCHEMA_NAMES_vec.begin(); it != MOTOR_SCHEMA_NAMES_vec.end(); it++) {

        if ((*it).compare(sender_s) == 0) { // check if the sender_s is listed

            return true;

        }

    }

    return false;

}

/// checkRotValue_v
/// \param vectorAbs_d
/// \brief Check the orientation_d for maximum value
void checkRotValue_v(double &orientation_d) {

    if (abs(orientation_d) > MAX_MOTION_VEC_ROT_d) { // avoid motion commands greater than the maximum

        auto sign_i = sgn_i<double>(orientation_d); // determine the sign of "orientation_d"

        orientation_d = sign_i * MAX_MOTION_VEC_ROT_d;

    }

}

/// checkTransValue_v
/// \param vectorAbs_d
/// \brief Check the vectorAbs_d for maximum value
void checkTransValue_v(double &vectorAbs_d) {

    if (vectorAbs_d > MAX_MOTION_VEC_ABS_d) { // avoid motion commands greater than the maximum

        vectorAbs_d = MAX_MOTION_VEC_ABS_d;

    }

}

/// gotInterrupt_b
/// \param motionVec_c
/// \return true if interrupt message, else false
/// \brief Check for interrupt messages
bool gotInterrupt_b(MotionVector motionVec_c) {

    return motionVec_c.type == MotionVecMsgType::INTERRUPT;

}

/// checkLegality_b
/// \param motionVec_c
/// \return true if illegal message, else false
/// \brief Check for illegal messages
bool checkLegality_b(MotionVector motionVec_c) {

    return motionVec_c.type == MotionVecMsgType::ILLEGAL;

}

/// mergeMotionVecBuffer_s
/// \param motionVec_mp
/// \return MotionVector
/// \brief Merge the motion vector buffer
MotionVector mergeMotionVecBuffer_s(std::map<std::string, MotionVector>& motionVec_mp) {

    MotionVector motionVecBuffer_s = DEFAULT_MOTION_VEC_s; // set default value

    float xBuffer_d = 0;
    float yBuffer_d = 0;

    bool legalMotionVecBuffer_b = false;

//    ROS_INFO("motionVec_mp.size() = %i", motionVec_mp.size()); // Debug only!

//    ROS_INFO("[%s]: ------------------------------------------------------------------------------",
//            ros::this_node::getName().c_str()); // Debug only!

    for (auto it = motionVec_mp.begin(); it != motionVec_mp.end(); ++it) {

        MotionVector motionVec_s = (*it).second;

//        ROS_INFO("[%s]: sender_s = %s, motionVec_s.x = %f, motionVec_s.y = %f", ros::this_node::getName().c_str(),
//                 (*it).first.c_str(), motionVec_s.x, motionVec_s.y); // Debug only!

        if (gotInterrupt_b(motionVec_s)) { // check for interrupt message

            return DEFAULT_MOTION_VEC_s; // return the default motion vector {dir,mag,...} = {0.0,0.0, MotionVecMsgType::ILLEGAL,...}

        } else if (checkLegality_b(motionVec_s)) { // check for illegal message

            continue; // skip illegal message

        } else {

            xBuffer_d += motionVec_s.x; // update the x buffer
            yBuffer_d += motionVec_s.y; // update the y buffer

            if (!legalMotionVecBuffer_b) { // if at least one message is legal, turn the buffer to a legal one

                legalMotionVecBuffer_b = true;

            }

        }

    }

//    ROS_INFO("[%s]: ==> xBuffer_d = %f, yBuffer_d = %f", ros::this_node::getName().c_str(), xBuffer_d, yBuffer_d); // Debug only!
//    ROS_INFO("[%s]: ------------------------------------------------------------------------------\n",
//            ros::this_node::getName().c_str()); // Debug only!

//    ROS_INFO("[%s] xBuffer_d = %f, yBuffer_d = %f", ros::this_node::getName().c_str(), xBuffer_d,
//             yBuffer_d); // Debug only!

    if (legalMotionVecBuffer_b) { // if the buffer is legal, turn the type to "COMMON"

        motionVecBuffer_s.type = MotionVecMsgType::COMMON;

    } else { // else turn the type to "ILLEGAL"

        motionVecBuffer_s.type = MotionVecMsgType::ILLEGAL;

    }

    motionVecBuffer_s.x = xBuffer_d; // set the x value
    motionVecBuffer_s.y = yBuffer_d; // set the y value

    return motionVecBuffer_s;

}

/// PUBLIC METHODS

/**
 * Constructor
 * ------------------------------------------------------
 * additional information are given in the MoveRobot.h
 * ------------------------------------------------------
*/
MoveRobot::MoveRobot(uint64_t id_ui) : MotorSchema(id_ui, MOTOR_SCHEMA_MOVE_ROBOT_NAME_str), getMotionVector_c(MOTION_VEC_TOPIC_st), finalVec_s(DEFAULT_MOTION_VEC_s),
                                       getPosition_c(ODOM_AMIRO_TOPIC_st), isStartTimeSet(false), timer_c(ros::Time::now().nsec) {

    this->pubMotionVec_c = this->nh_c.advertise<geometry_msgs::Twist>(TWIST_TOPIC_st, 1); // advertise the robot motion publishing topic

    this->pubFinalGradient_c = this->nh_c.advertise<geometry_msgs::Twist>(FINAL_GRADIENT_TOPIC_st, 1); // advertise the robot gradient publishing topic

}

/**
 * Destructor
 * ------------------------------------------------------
 * additional information are given in the MoveRobot.h
 * ------------------------------------------------------
*/
MoveRobot::~MoveRobot() {

}

/**
 * Publish the motion commands
 * ------------------------------------------------------
 * additional information are given in the MoveRobot.h
 * ------------------------------------------------------
*/
bool MoveRobot::executeBehavior_b() {

    if (!this->isStartTimeSet) { // check if the start time is set

        this->startTime = ros::Time::now(); // set the start time of the listen loop

        this->isStartTimeSet = true;

    }

    this->timer_c = ros::Time::now() - this->startTime; // update the timer

    if (this->timer_c.nsec <= MAX_NS_TO_LISTEN_i) { // check if the current loop has not finished

        this->listen_v(this->getMotionVector_c.getPerceptualData_s()); // buffer the current motion vector

    } else {  // current loop finished

        this->finalVec_s = mergeMotionVecBuffer_s(this->motionVec_mp); // merge the motion vector to a final motion vector

        this->motionVec_mp.clear(); // clear the motion vector buffer

        this->publishMotionVector_v(this->finalVec_s); // publish the final motion vector

        this->finalVec_s = DEFAULT_MOTION_VEC_s; // set default value

        this->isStartTimeSet = false;

    }

    return true;

}

/**
 * Publish a stop commands
 * ------------------------------------------------------
 * additional information are given in the MoveRobot.h
 * ------------------------------------------------------
*/
void MoveRobot::stopRobot_v() {

    geometry_msgs::Twist twist_c;

    twist_c.linear.x = 0.0; // stop message translation
    twist_c.angular.z = 0.0; // stop message rotation

    this->pubMotionVec_c.publish(twist_c); // publish the motion respectively twist command to the robot

}

/// PROTECTED METHODS

/**
 * Publish the output vector (behavior()) to the robot
 * ------------------------------------------------------
 * additional information are given in the MoveRobot.h
 * ------------------------------------------------------
*/
void MoveRobot::publishMotionVector_v(MotionVector motionVec_s){

    geometry_msgs::Twist twist_c;
    geometry_msgs::Twist finalGradient_c;

    if (checkLegality_b(motionVec_s)) { // cause stopping if the message is not legal

        twist_c.linear.x = 0.0; // stop message translation
        twist_c.angular.z = 0.0; // stop message rotation

        // keep the visualization equal
        finalGradient_c.linear.x = twist_c.linear.x;
        finalGradient_c.angular.z = twist_c.angular.z;

    } else {

//        ROS_INFO("[%s]: motionVec_s.x = %f, motionVec_s.y = %f", ros::this_node::getName().c_str(), motionVec_s.x, motionVec_s.y); // Debug only!

        double vectorAbs_d = sqrt(motionVec_s.y * motionVec_s.y + motionVec_s.x * motionVec_s.x); // determine the vector magnitude

        double vectorAngle_d = atan2(motionVec_s.y, motionVec_s.x); // determine the vector direction

        double angleDiff_d = getAngleDiff(this->getPosition_c.getPositionByFrame_c(FRAME_MAP_st).theta, vectorAngle_d); // determine the angle difference

        checkTransValue_v(vectorAbs_d); // keep the translation value of the vector in adequate boundaries

        checkRotValue_v(angleDiff_d); // keep the rotation value of the vector in adequate boundaries

        twist_c.linear.x = TRANS_VEL_SCALE_MS_d * vectorAbs_d; // scale the motion command and choose the x axis as driving direction

        twist_c.angular.z = ROT_VEL_SCALE_RS_d * angleDiff_d; // scale the rotation around the z axis

        // ToDo: Source this out into a function or the feedback
        double factorLin_d = (1.5-abs(angleDiff_d)/MAX_MOTION_VEC_ROT_d);
        double factorAng_d = (1.75-abs(angleDiff_d)/MAX_MOTION_VEC_ROT_d);
        twist_c.linear.x = factorLin_d * twist_c.linear.x; // Debug only!
        twist_c.angular.z = factorAng_d * twist_c.angular.z; // Debug only!

        finalGradient_c.linear.x = vectorAbs_d;

        finalGradient_c.angular.z = vectorAngle_d;

//        ROS_INFO("[%s]: vectorAbs = %f, angleDiff = %f, orientation = %f, vectorAngle = %f", ros::this_node::getName().c_str(), vectorAbs_d,
//                 angleDiff_d, this->getPosition_c.getPositionByFrame_c(FRAME_MAP_st).theta, vectorAngle_d); // Debug only!

    }

    this->pubMotionVec_c.publish(twist_c); // publish the motion respectively twist command to the robot

    this->pubFinalGradient_c.publish(finalGradient_c); // publish the final gradient vector to the visualization node

}

/// PRIVATE METHODS

/**
 * Manage the incoming motion vectors
 * ------------------------------------------------------
 * additional information are given in the MoveRobot.h
 * ------------------------------------------------------
*/
void MoveRobot::listen_v(MotionVector motionVec_s) {

    std::string sender_s = motionVec_s.sender;

    if (!checkSender_b(sender_s)) { // check the sender name

        return; // quit if the sender name is illegal

    }

//    ROS_INFO("[%s]: sender_s = %s",ros::this_node::getName().c_str(), sender_s.c_str()); // Debug only!

//    ROS_INFO("[%s]: motionVec_s.x = %f, motionVec_s.y = %f", ros::this_node::getName().c_str(), motionVec_s.x, motionVec_s.y); // Debug only!

    this->motionVec_mp[sender_s] = motionVec_s; // buffer the motion vector with the associated sender name

}