/**
 * ==============================================================================
 * \file StayOnPath.cpp
 * \brief Provides the definition of the motor schema class StayOnPath
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/motor_schemas/StayOnPath.h"

/// NAMESPACES

using namespace std;

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ------------------------------------------------------
 * additional information are given in the StayOnPath.h
 * ------------------------------------------------------
*/
StayOnPath::StayOnPath(uint64_t id_ui, tf::TransformListener *listener_c) : MotorSchema(id_ui, MOTOR_SCHEMA_STAY_ON_PATH_NAME_str), getRFID_c(RFID_TAG_TOPIC_st),
                                                                            getPosition_c(ODOM_AMIRO_TOPIC_st), initialRFID_ui(DEFAULT_RFID_i), targetRFID_ui(targetRFID_ui),
                                                                            imageTransport_c(this->nh_c), getAlignment_c(), listener_c(listener_c) {

    this->pubMotionVec_c = this->nh_c.advertise<aal_potential_field_navigation::motionVectorMessage>(MOTION_VEC_TOPIC_st, 1); // advertise the general motion vector publishing topic

    this->vectorfieldPub_c = this->imageTransport_c.advertise(VEC_FIELD_ALIGN_TOPIC_st, 1, true); // advertise the vectorfield topic of the visualization (vectorfield_to_gridmap) node

//    this->vectorfieldPub_c = this->imageTransport_c.advertise(VEC_FIELD_FINAL_TOPIC_st, 1, true); // advertise the vectorfield topic of the visualization (vectorfield_to_gridmap) node

}

/**
 * Destructor
 * ------------------------------------------------------
 * additional information are given in the StayOnPath.h
 * ------------------------------------------------------
*/
StayOnPath::~StayOnPath() {

}

/**
 * Check for localization
 * ------------------------------------------------------
 * additional information are given in the MoveToTarget.h
 * ------------------------------------------------------
*/
bool StayOnPath::isLocalized_b() {

    bool gotRFID_b = this->getRFID_c.getInitialRFID_ui() != DEFAULT_RFID_i; // check RFID

    bool gotPos_b = this->getPosition_c.getPosition_c().second != DEFAULT_POS_f && this->getPosition_c.getPosition_c().first != DEFAULT_POS_f; // check position

    return (gotPos_b && gotRFID_b);

}

/**
 * Initiate the motor schema
 * ------------------------------------------------------
 * additional information are given in the StayOnPath.h
 * ------------------------------------------------------
*/
bool StayOnPath::init_b() {

    if (!this->isLocalized_b()) { // paranoia, because the init(ialize) method need a previous localization

        return false;

    }

    std::pair<uint64_t, uint64_t> RFIDs = this->getRFID_c.getPerceptualData_pair(); // get the last RFID (.first) and the current RFID (.second)

    this->initialRFID_ui = this->getRFID_c.getInitialRFID_ui(); // get the first detected RFID

    if (!this->getAlignment_c.getPerceptualData_b(this->vectorfield_mat, this->targetRFID_ui, RFIDs.second, RFIDs.first,
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
 * additional information are given in the StayOnPath.h
 * ------------------------------------------------------
*/
bool StayOnPath::executeBehavior_b() {

    float posX_f = this->getPosition_c.getPosition_c().first; // get the current x coordinate
    float posY_f = this->getPosition_c.getPosition_c().second; // get the current y coordinate

//    ROS_INFO("[%s]: xMap = %f, yMap = %f", ros::this_node::getName().c_str(), posX_f, posY_f); // Debug only!

    cv::Point2i pose2d_c = this->mapPoseToVectorfield_c(); // get the current pose inside the vectorfield

//    ROS_INFO("[%s]: pose2d_c.x = %i, pose2d_c.y = %i", ros::this_node::getName().c_str(), pose2d_c.x, pose2d_c.y); // Debug only!

    if (pose2d_c == DEFAULT_POINT_c) { // check for mapping error

        return false; // quit the behavior executing and cause an error

    }

    cv::Point2f vector_c(this->vectorfield_mat.at<cv::Vec2f>(pose2d_c.y, pose2d_c.x)[0], this->vectorfield_mat.at<cv::Vec2f>(pose2d_c.y, pose2d_c.x)[1]); // get the motion vector from the vectorfield

    flipVector_v(vector_c); // transform the vector into the world frame

//    ROS_INFO("[%s]: vector_c.x = %f, vector_c.y = %f", ros::this_node::getName().c_str(), vector_c.x, vector_c.y); // Debug only!

    this->publishMotionVector_v(vector_c, MotionVecMsgType ::COMMON, DEFAULT_MOTION_VEC_PRIO_i, this->name_s); // publish the motion vector

    return true;

}

/**
 * Publish the whole vector field to the
 * vectorfield_to_gridmap node
 * ------------------------------------------------------
 * additional information are given in the StayOnPath.h
 * ------------------------------------------------------
*/
void StayOnPath::visualizeVectorfield_v() {

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

/**
 * Setter: targetRFID_ui
 * ------------------------------------------------------
 * additional information are given in the StayOnPath.h
 * ------------------------------------------------------
*/
void StayOnPath::setTargetRFID_v(uint64_t targetRFID_ui) {

    this->targetRFID_ui = targetRFID_ui;

}

/// PROTECTED METHODS

/// PRIVATE METHODS

/**
 * Map the pose respectively the position to the
 * vectorfield
 * ------------------------------------------------------
 * additional information are given in the StayOnPath.h
 * ------------------------------------------------------
*/
cv::Point2i StayOnPath::mapPoseToVectorfield_c() {

    cv::Point2i pntBuffer_c;

    float posX_f = this->getPosition_c.getPosition_c().first;
    float posY_f = this->getPosition_c.getPosition_c().second;

    pntBuffer_c = DEFAULT_POINT_c; // set a default value

    geometry_msgs::Pose odomPose_tf;

    if (PIXEL_MODE_i) {

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