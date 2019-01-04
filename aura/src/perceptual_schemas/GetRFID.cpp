/**
 * ==============================================================================
 * \file  GetRFID.cpp
 * \brief Provides the definition of the perceptual schema class GetRFID
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/perceptual_schemas/GetRFID.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ---------------------------------------------------------
 * additional information are given in the GetRFID.h
 * ---------------------------------------------------------
*/
GetRFID::GetRFID(string topic_st) : PerceptualSchema(topic_st), currentRFID_ui(DEFAULT_RFID_i),
                                    lastRFID_ui(DEFAULT_RFID_i), initialRFID_ui(DEFAULT_RFID_i),
                                    currentRFIDBuffer_ui(DEFAULT_RFID_i) {

    this->subRfidTagList_c = this->nh_c.subscribe<std_msgs::UInt32MultiArray>(this->topic_st, 1,
                                                                              &GetRFID::RFIDCallback_v, this); // set up the subscriber and bind the "rfidTagListCallback" to the "topic"

    this->getRFIDTypeServiceClient_c = this->nh_c.serviceClient<aal_potential_field_navigation::get_rfid_type>(GET_RFID_TYPE_st); // advertise the "check_rfid" service to get some information about RFID tags

}

/**
 * Destructor
 * ---------------------------------------------------------
 * additional information are given in the GetRFID.h
 * ---------------------------------------------------------
*/
GetRFID::~GetRFID() {

}

/**
 * Return the specific perceptual data
 * [0]: lastRFID
 * [1]: currentRFID
 * ---------------------------------------------------------
 * additional information are given in the GetRFID.h
 * ---------------------------------------------------------
*/
std::pair<uint64_t, uint64_t> GetRFID::getPerceptualData_pair(){

    return std::make_pair(this->lastRFID_ui, this->currentRFID_ui);

}

/**
 * Return the first detected RFID
 * ---------------------------------------------------------
 * additional information are given in the GetRFID.h
 * ---------------------------------------------------------
*/
uint64_t GetRFID::getInitialRFID_ui() {

    return this->initialRFID_ui;

}

/// PROTECTED METHODS

/// PRIVATE METHODS

/**
 * Callback function which is dedicated to the perceptual
 * schema specific topic
 * ---------------------------------------------------------
 * additional information are given in the GetRFID.h
 * ---------------------------------------------------------
*/
void GetRFID::RFIDCallback_v(const std_msgs::UInt32MultiArray::ConstPtr &msg_c){

    if (msg_c->data.empty()) { // check if there are RFID tags in range

        this->currentRFID_ui = DEFAULT_RFID_i; // reset the current RFID every call if there are no RFID tags in range


    } else {

        for (auto it_c = msg_c->data.begin(); it_c != msg_c->data.end(); ++it_c) { // get the present RFID-Tag(s)

            this->currentRFID_ui = *it_c; // as convention there is only one tag present at once, so copy the RFID-Tag into data_ui

        }

    }

    if (this->initialRFID_ui == DEFAULT_RFID_i) { // store the first detected RFID as initial RFID

        this->initialRFID_ui = this->currentRFID_ui;

    }

    if (this->currentRFID_ui != DEFAULT_RFID_i && this->currentRFIDBuffer_ui == DEFAULT_RFID_i) { // only store a last RFID if the current does not register the RFID anymore

        this->currentRFIDBuffer_ui = this->currentRFID_ui;

        return;

    } else if (this->currentRFID_ui == DEFAULT_RFID_i && this->currentRFIDBuffer_ui != DEFAULT_RFID_i) {

        this->lastRFID_ui = this->currentRFIDBuffer_ui; // store the last currentRFID as new lastRFID

        this->currentRFIDBuffer_ui = DEFAULT_RFID_i;

    } else {

        return;

    }

//    ROS_INFO("lastRFID_ui = %i, currentRFID_ui = %i, initialRFID_ui = %i", this->lastRFID_ui, this->currentRFID_ui,
//             this->initialRFID_ui); // Debug only

}

/**
 * Return the type of the current RFID tag
 * ---------------------------------------------------------
 * additional information are given in the GetRFID.h
 * ---------------------------------------------------------
*/
RFIDType GetRFID::getCurrentRFIDType_e() {

    aal_potential_field_navigation::get_rfid_type srv_c; // create a service handle

    // define the service call
    srv_c.request.currentRFID_ui = this->currentRFID_ui;

    this->getRFIDTypeServiceClient_c.call(srv_c); // call the service
    ros::service::waitForService(GET_RFID_TYPE_st); // wait for service response

    if (!srv_c.response.jobDone_i) { // check service response

        this->errorLog_lst.push_back((ErrorCode)srv_c.response.errorCode_i); // log the error

        return RFIDType::DEFAULT; // return the default type

    } else {

        return (RFIDType)srv_c.response.RFIDType_i;

    }

}

