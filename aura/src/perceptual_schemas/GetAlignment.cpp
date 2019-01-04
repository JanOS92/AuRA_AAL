/**
 * ==============================================================================
 * \file GetAlignment.cpp
 * \brief Provides the definition of the perceptual schema class GetAlignment
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/perceptual_schemas/GetAlignment.h"

// Utils
#include "aal_potential_field_navigation/utils/image_converting_utils.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * -----------------------------------------------------------------
 * additional information are given in the GetAlignment.h
 * -----------------------------------------------------------------
*/
GetAlignment::GetAlignment() : PerceptualSchema(DEFAULT_TOPIC_st), targetRFID_ui(DEFAULT_RFID_i),
                               initialRFID_ui(DEFAULT_RFID_i), fieldKeyBuffer_st(DEFAULT_FIELD_KEY_st) {

    this->getFieldByKeyServiceClient_c = this->nh_c.serviceClient<aal_potential_field_navigation::get_field_by_key>(GET_FIELD_BY_KEY_st); // advertise the "get_field_by_key" service to call for vectorfields

    this->checkRFIDServiceClient_c = this->nh_c.serviceClient<aal_potential_field_navigation::check_rfid>(CHECK_RFID_st); // advertise the "check_rfid" service to get some information about RFID tags

}

/**
 * Destructor
 * -----------------------------------------------------------------
 * additional information are given in the GetAlignment.h
 * -----------------------------------------------------------------
*/
GetAlignment::~GetAlignment() {

}

/**
 * Return the specific perceptual data
 * ---------------------------------------------------------
 * additional information are given in the GetAlignment.h
 * ---------------------------------------------------------
*/
bool GetAlignment::getPerceptualData_b(cv::Mat& vectorfield, uint64_t targetRFID_ui, uint64_t currentRFID_ui, uint64_t lastRFID_ui, uint64_t initialRFID_ui) {

    string fieldKey_st = DEFAULT_FIELD_KEY_st;

    if (!generateFieldKey_b(fieldKey_st, targetRFID_ui, currentRFID_ui, initialRFID_ui, lastRFID_ui)) {

        this->errorLog_lst.push_back(GENERATE_FIELD_KEY_ERROR); // log the error

        return false; // quit the method with an error

    }

    if (fieldKey_st == DEFAULT_FIELD_KEY_st) { // check the field key

        DEFAULT_VECTORFIELD_mat.copyTo(vectorfield); // copy the default/zero vectorfield

    } else if (fieldKey_st == this->fieldKeyBuffer_st) {

        return true; // quit the method successfully without a vectorfield update

    } else {

        this->fieldKeyBuffer_st = fieldKey_st; // buffer the current field key

        aal_potential_field_navigation::get_field_by_key srv_c; // service client

        // define the service call
        srv_c.request.fieldTypeKeyword_st = FIELD_TYPE_KEYWORD_VEC_st;
        srv_c.request.fieldImageName_st = fieldKey_st;
        srv_c.request.fieldNamePostfix_st = FIELD_NAME_POSTFIX_ALIGN_st;

        this->getFieldByKeyServiceClient_c.call(srv_c); // call the service
        ros::service::waitForService(GET_FIELD_BY_KEY_st); // wait for service response

        if(!srv_c.response.jobDone_i) { // check service response

            this->errorLog_lst.push_back((ErrorCode)srv_c.response.errorCode_i); // log the error

            return false; // quit the method with an error

        }

        cv_bridge::CvImageConstPtr cvPtr_c = cv_bridge::toCvCopy(srv_c.response.field_c, sensor_msgs::image_encodings::TYPE_32FC2);

        cvPtr_c->image.copyTo(vectorfield);

    }

//    ROS_INFO("fieldKey_st = %s", fieldKey_st.c_str()); // Debug only

    return true; // quit the method successfully

}

/**
 * Check the current RFID
 * ---------------------------------------------------------
 * additional information are given in the GetAlignment.h
 * ---------------------------------------------------------
*/
bool GetAlignment::checkRFID_b(aal_potential_field_navigation::check_rfid &srv_c, uint64_t currentRFID_ui, uint64_t lastRFID_ui) {

    // define the service call
    srv_c.request.initialRFID_ui = this->initialRFID_ui;
    srv_c.request.targetRFID_ui = this->targetRFID_ui;
    srv_c.request.currentRFID_ui = currentRFID_ui;
    srv_c.request.lastRFID_ui = lastRFID_ui;

    this->checkRFIDServiceClient_c.call(srv_c); // call the service
    ros::service::waitForService(CHECK_RFID_st); // wait for service response

//    ROS_INFO("[%s]: srv_c.response.jobDone_i = %i, srv_c.response.located_i = %i", ros::this_node::getName().c_str(),
//             srv_c.response.jobDone_i, srv_c.response.located_i); // Debug only!

    if (!srv_c.response.jobDone_i) { // check service response

        this->errorLog_lst.push_back((ErrorCode)srv_c.response.errorCode_i); // log the error

        return false; // quit the method with an error

    } else if (!srv_c.response.located_i) {

        return false; // quit the method without causing an update

    }

    return true;

}

/// PROTECTED METHODS


/// PRIVATE METHODS

/**
 * Generate a field key according to the current RFID tag
 * -----------------------------------------------------------------
 * additional information are given in the GetAlignment.h
 * -----------------------------------------------------------------
*/
bool GetAlignment::generateFieldKey_b(string &fieldKey_st, uint64_t targetRFID_ui, uint64_t currentRFID_ui, uint64_t initialRFID_ui, uint64_t lastRFID_ui) {

    // store the initial and target RFID for lifetime
    this->initialRFID_ui = initialRFID_ui;
    this->targetRFID_ui = targetRFID_ui;

//    ROS_INFO("[%s]: initialRFID_ui = %i, currentRFID_ui = %i , lastRFID_ui = %i", ros::this_node::getName().c_str(), initialRFID_ui,
//             currentRFID_ui, lastRFID_ui); // Debug only!

    fieldKey_st = ""; // reset the fieldKey reference

    aal_potential_field_navigation::check_rfid srv_c; // create a service handle

    if (!this->checkRFID_b(srv_c, currentRFID_ui, lastRFID_ui)) { // check RFID

//        ROS_INFO("[%s]: checkRFID_b failed", ros::this_node::getName().c_str()); // Debug only!

        return false; // quit the method without causing an update

    }

//    ROS_INFO("[%s]: RFIDAction)srv_c.response.action_i = %i", ros::this_node::getName().c_str(),
//             (RFIDAction) srv_c.response.action_i); // Debug only!

    fieldKey_st = rfidActionToFieldKey_st((RFIDAction)srv_c.response.action_i, targetRFID_ui, initialRFID_ui);

//    ROS_INFO("[%s]: fieldKey_st = %s", ros::this_node::getName().c_str(), fieldKey_st.c_str()); // Debug only!

    return true;

}