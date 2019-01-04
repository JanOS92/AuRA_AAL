/**
 * ==============================================================================
 * \file MoveToTarget.cpp
 * \brief Provides the definition of the motor schema class MoveToTarget
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/motor_schemas/MoveToTarget.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES
bool goalDetected_b = false;

/// LOCAL FUNCTIONS

/// goalReached_b
/// \param floorLeftSideValue_ui
/// \param floorRightSideValue_ui
/// \return true if the goal is reached else false
bool goalReached_b(uint16_t floorLeftSideValue_ui, uint16_t floorRightSideValue_ui) {

    if (floorLeftSideValue_ui <= BLACK_LINE_i &&
        floorRightSideValue_ui <= BLACK_LINE_i) { // check if the sensor values are identify the goal bar

        return true;

    } else {

        return false;

    }

}

/// PUBLIC METHODS

/**
 * Constructor
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
MoveToTarget::MoveToTarget(uint64_t id_ui, uint64_t targetRFID_ui, JobLogic jobLogic_e, moveToTargetActionServer_c* actionServer_pc, tf::TransformListener *listener_c) :
                                                                MotorSchema(id_ui, MOTOR_SCHEMA_MOVE_TO_TARGET_NAME_str), getRFID_c(RFID_TAG_TOPIC_st),
                                                                getDirection_c(), targetRFID_ui(targetRFID_ui), getPosition_c(ODOM_AMIRO_TOPIC_st),
                                                                jobLogic_e(jobLogic_e), actionServer_pc(actionServer_pc), initialRFID_ui(DEFAULT_RFID_i),
                                                                imageTransport_c(this->nh_c), getFloorValues_c(PROXIMITY_FLOOR_TOPIC_st), checkedRFID_ui(DEFAULT_RFID_i),
                                                                listener_c(listener_c) {

    this->pubMotionVec_c = this->nh_c.advertise<aal_potential_field_navigation::motionVectorMessage>(MOTION_VEC_TOPIC_st, 1); // advertise the general motion vector publishing topic

    this->vectorfieldPub_c = this->imageTransport_c.advertise(VEC_FIELD_DIR_TOPIC_st, 1, true); // advertise the vectorfield topic of the visualization (vectorfield_to_gridmap) node

    this->setCheckJobLogic_v(); // call for default initialization

}

/**
 * Destructor
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
MoveToTarget::~MoveToTarget() {

}

/**
 * Check for localization
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
bool MoveToTarget::isLocalized_b() {

    bool gotRFID_b = this->getRFID_c.getInitialRFID_ui() != DEFAULT_RFID_i; // check RFID

    bool gotPos_b = this->getPosition_c.getPosition_c().second != DEFAULT_POS_f && this->getPosition_c.getPosition_c().first != DEFAULT_POS_f; // check position

    return (gotPos_b && gotRFID_b);

}

/**
 * Calculate the output vector as output of the underlying
 * schema (behavior)
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
int i = 0;
bool MoveToTarget::init_b() {

    if (!this->isLocalized_b()) { // paranoia, because the init(ialize) method need a previous localization

        return false;

    }

    this->setCheckJobLogic_v(); // set check job logic again, because a new target maybe needs a different check job logic

    std::pair<uint64_t, uint64_t> RFIDs = this->getRFID_c.getPerceptualData_pair(); // get the last RFID (.first) and the current RFID (.second)

    this->initialRFID_ui = this->getRFID_c.getInitialRFID_ui(); // get the first detected RFID

    if (!this->getDirection_c.getPerceptualData_b(this->vectorfield_mat, this->targetRFID_ui, RFIDs.second, RFIDs.first,
                                                  this->initialRFID_ui)) { // write the necessary vectorfield into the member variable "vectorfield"

        this->errorLog_lst.push_back(ErrorCode::BEHAVIOR_ERROR); // log the error

        return false; // quit with causing an error

    }

    this->visualizeVectorfield_v(); // visualize the vectorfield in RViz

    return true;

}

/**
 * Publish the motion commands
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
bool MoveToTarget::executeBehavior_b() {

    float posX_f = this->getPosition_c.getPosition_c().first; // get the current x coordinate
    float posY_f = this->getPosition_c.getPosition_c().second; // get the current y coordinate

//    ROS_INFO("[%s]: xMap = %f, yMap = %f", ros::this_node::getName().c_str(), posX_f, posY_f); // Debug only!

    cv::Point2i pose2d_c = this->mapPoseToVectorfield_c(); // get the current pose inside the vectorfield

//    ROS_INFO("[%s]: pose2d_c.x = %i, pose2d_c.y = %i", ros::this_node::getName().c_str(), pose2d_c.x, pose2d_c.y); // Debug only!

    if (pose2d_c == DEFAULT_POINT_c) { // check for mapping error

        return false; // quit the behavior executing and cause an error

    }

    cv::Point2f vector_c(this->vectorfield_mat.at<cv::Vec2f>(pose2d_c.y, pose2d_c.x)[0],
                         this->vectorfield_mat.at<cv::Vec2f>(pose2d_c.y, pose2d_c.x)[1]); // get the motion vector from the vectorfield

    flipVector_v(vector_c); // transform the vector into the world frame

//    ROS_INFO("[%s]: vector_c.x = %f, vector_c.y = %f", ros::this_node::getName().c_str(), vector_c.x, vector_c.y); // Debug only!

    this->publishMotionVector_v(vector_c, MotionVecMsgType ::COMMON, DEFAULT_MOTION_VEC_PRIO_i, this->name_s); // publish the motion vector

    return true;

}

/**
 * Check the job status
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::checkJob_v() {

    this->checkJobLogic_v(); // call the specific check job logic

}

/**
 * Setter: jobLogic_e
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::setJobLogic_v(JobLogic jobLogic_e) {

    this->jobLogic_e = jobLogic_e;

}

/**
 * Setter: targetRFID_ui
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::setTargetRFID_v(uint64_t targetRFID_ui) {

    this->targetRFID_ui = targetRFID_ui;

}

/**
 * Publish the whole vector field to the
 * vectorfield_to_gridmap node
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::visualizeVectorfield_v() {

    cv_bridge::CvImage cvImageVec_c;

    cvImageVec_c.encoding = sensor_msgs::image_encodings::TYPE_32FC2; // set the message encoding type
    cvImageVec_c.image = this->vectorfield_mat; // set the vectorfield as image

    ROS_INFO("[%s]: wait for visualization nodes ...", ros::this_node::getName().c_str());

    ros::Time startTime = ros::Time::now();
    ros::Duration timer_c;

    while (this->vectorfieldPub_c.getNumSubscribers() == 0) { // wait for visualization nodes to subscribe

        timer_c = ros::Time::now() - startTime; // update the timer

        if (timer_c.sec >= MAX_NS_TO_WAIT_FOR_SUB_i) {

            ROS_INFO("[%s]: timeout while waiting for visualization nodes ...", ros::this_node::getName().c_str());

            return;

        }

    }

    ROS_INFO("[%s]: visualize the vectorfield ...", ros::this_node::getName().c_str());

    this->vectorfieldPub_c.publish(cvImageVec_c.toImageMsg()); // publish the vectorfield

}

/// PROTECTED METHODS

/// PRIVATE METHODS

/**
 * Map the pose respectively the position to the
 * vectorfield
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
cv::Point2i MoveToTarget::mapPoseToVectorfield_c() {

    cv::Point2i pntBuffer_c;

    pntBuffer_c = DEFAULT_POINT_c; // set a default value

    geometry_msgs::Pose odomPose_tf;

    if (PIXEL_MODE_i) {

        float posX_f = this->getPosition_c.getPosition_c().first;
        float posY_f = this->getPosition_c.getPosition_c().second;

        pntBuffer_c = cv::Point2i((int) (posX_f * PIXEL_SCALE_d), (int) (posY_f * PIXEL_SCALE_d));

    } else {

        tf::StampedTransform transform_c; // transformation between the robot and the vector field

        try {

            this->listener_c->waitForTransform(FRAME_POT_FIELD_st, FRAME_AMIRO_BASE_LINK_st, ros::Time(0), TIME_TO_WAIT_OF_STP_c); // wait for transformation

            this->listener_c->lookupTransform(FRAME_POT_FIELD_st, FRAME_AMIRO_BASE_LINK_st, ros::Time(0), transform_c); // apply transformation between the tfs

        } catch (tf::TransformException ex) {

            ROS_ERROR("[%s] %s", ros::this_node::getName().c_str(), ex.what());

            return DEFAULT_POINT_c; // if an error occurs, return the default value

        }

        odomPose_tf.position.x = transform_c.getOrigin().x(); // get the new x coordinate inside the vectorfield frame
        odomPose_tf.position.y = transform_c.getOrigin().y(); // get the new y coordinate inside the vectorfield frame

        pntBuffer_c = cv::Point2i(pose2pixel(odomPose_tf, this->vectorfield_mat.cols, this->vectorfield_mat.rows,
                                             METER_PER_PIXEL_WIDTH_d, METER_PER_PIXEL_HEIGHT_d)); // get the new pose inside the vectorfield

    }

    if (pntBuffer_c.x < 0 || pntBuffer_c.x >= this->vectorfield_mat.cols || pntBuffer_c.y < 0 ||
        pntBuffer_c.y >= this->vectorfield_mat.rows) {

        ROS_WARN("[%s] Current AMiRo (%f,%f) is not in image (%i,%i).", ros::this_node::getName().c_str(), odomPose_tf.position.x, odomPose_tf.position.y, pntBuffer_c.x,
                 pntBuffer_c.y);

        return DEFAULT_POINT_c; // if the pose is not inside the given bounds, return the default value

    }

//    ROS_INFO("[%s] Current AMiRo location in potential field (%f,%f) and in image (%i,%i).", ros::this_node::getName().c_str(), odomPose_tf.position.x, odomPose_tf.position.y, pntBuffer_c.x,
//             pntBuffer_c.y); // Debug only!

    return pntBuffer_c;

}

/**
 * Check the default job status (target reached)
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::checkJobDefault_v() {

    std::pair<uint64_t, uint64_t> RFIDs = this->getRFID_c.getPerceptualData_pair(); // get the last RFID (.first) and the current RFID (.second)

    uint64_t lastRFID_ui = RFIDs.first;
    uint64_t currentRFID_ui = RFIDs.second;

    if (!this->RFIDChecked_b(currentRFID_ui)) { // only check the job if the RFID is not checked already respectively new data arrives

        aal_potential_field_navigation::check_rfid srv_c; // create a service handle

        if (!this->getDirection_c.checkRFID_b(srv_c, currentRFID_ui, lastRFID_ui)) { // check RFID

            return; // quit the method early

        }

        this->notifyTargetReached_v((RFIDAction)srv_c.response.action_i); // send feedback
    }

}

/**
 * Check the specific job status (goal reached)
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::checkJobGoal_v() {

    std::pair<uint64_t, uint64_t> RFIDs = this->getRFID_c.getPerceptualData_pair(); // get the last RFID (.first) and the current RFID (.second)

    uint64_t lastRFID_ui = RFIDs.first;
    uint64_t currentRFID_ui = RFIDs.second;

    aal_potential_field_navigation::check_rfid srv_c; // create a service handle

    if (!this->RFIDChecked_b(currentRFID_ui)) { // only check the job if the RFID is not checked already respectively new data arrives

        goalDetected_b = false; // reset flag

        if (!this->getDirection_c.checkRFID_b(srv_c, currentRFID_ui, lastRFID_ui)) { // check RFID

            return; // quit the method early

        } else if ((RFIDAction) srv_c.response.action_i ==
                RFIDAction::STOP) { // check if the goal is in range (compare the current RFID action with the goal RFID action)

            goalDetected_b = true; // the goal RFID-Tag is in range

        } else if ((RFIDAction) srv_c.response.action_i ==
                RFIDAction::ASCEND) { // check if the goal is missed and a gateway is in range (compare the current RFID action with the gateway RFID action)

            this->notifyGoalMissed_v(); // send feedback

            return; // quit the method early

        } else {

            return; // quit the method early

        }

    }

    if (goalDetected_b) { // check if the goal is reached

        if (goalReached_b(this->getFloorValues_c.getPerceptualData_ui(FloorSensor::FLOOR_LEFT_SIDE),
                          this->getFloorValues_c.getPerceptualData_ui(FloorSensor::FLOOR_RIGHT_SIDE))) { // check the floor sensor values

            this->notifyGoalReached_v(); // send feedback

            goalDetected_b = false; // reset flag

        }

    }

}

/**
 * Set the checkJobLogic_b member variable in dependency
 * of the
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::setCheckJobLogic_v() {

    if (this->jobLogic_e == JobLogic::GOAL_JOB) { // bind the check job logic in dependency of the expected RFID action

        this->checkJobLogic_v = bind(&MoveToTarget::checkJobGoal_v, this);

    } else {

        this->checkJobLogic_v = bind(&MoveToTarget::checkJobDefault_v, this);

    }

}

/**
 * Check if the input RFID is already checked
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
bool MoveToTarget::RFIDChecked_b(uint64_t RFID_ui) {

    if (RFID_ui != this->checkedRFID_ui) { // RFID is not checked already

//        ROS_INFO("[%s]: RFID_ui = %i", ros::this_node::getName().c_str(), RFID_ui); // Debug only!

        this->checkedRFID_ui = RFID_ui;

        return false;

    } else { // RFID is already checked

        return true;

    }

}

/**
 * Send a feedback to the state machine that the goal is missed
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::notifyGoalMissed_v() {

    aal_potential_field_navigation::move_to_targetFeedback MoveToTargetFeedback_c; // create a feedback message

    MoveToTargetFeedback_c.RFIDAction_i = RFIDAction::ASCEND;

    this->actionServer_pc->publishFeedback(MoveToTargetFeedback_c); // publish the RFID action to the clients

}

/**
 * Send a feedback to the state machine that the goal is reached
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::notifyGoalReached_v() {

    aal_potential_field_navigation::move_to_targetFeedback MoveToTargetFeedback_c; // create a feedback message

    MoveToTargetFeedback_c.RFIDAction_i = RFIDAction::STOP;

    this->actionServer_pc->publishFeedback(MoveToTargetFeedback_c); // publish the RFID action to the clients

}

/**
 * Send a feedback to the state machine that the target is reached
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
void MoveToTarget::notifyTargetReached_v(RFIDAction RFIDAction_e) {

    aal_potential_field_navigation::move_to_targetFeedback MoveToTargetFeedback_c; // create a feedback message

    MoveToTargetFeedback_c.RFIDAction_i = RFIDAction_e;

    this->actionServer_pc->publishFeedback(MoveToTargetFeedback_c); // publish the RFID action to the clients

}