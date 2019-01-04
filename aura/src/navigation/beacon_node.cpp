/**
 * ==============================================================================
 * \file beacon_node.cpp
 * \brief Provides different services for localization and path planning
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

// ROS service
#include "../../../../../install/include/aal_potential_field_navigation/check_rfid.h"
#include "../../../../../install/include/aal_potential_field_navigation/get_rfid_type.h"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Utils
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/utils/utils.h"
#include "aal_potential_field_navigation/Constants.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

std::map<uint64_t, RFID> RFIDMap_mp;
bool mapRoutineDone_b = false;

/// LOCAL FUNCTIONS

/// initializeRFIDMap_b
/// \return true if succeed else false
bool initializeRFIDMap_b() {

    ROS_INFO("[%s]: building map representation...", ros::this_node::getName().c_str());

    uint64_t keyValue_ui = DEFAULT_RFID_i;

    uint64_t idBuffer_ui = DEFAULT_RFID_i;
    vector<uint64_t> predecessorsBuffer_vec;
    vector<uint64_t> successorsBuffer_vec;
    RFIDType typeBuffer_st = RFIDType::DEFAULT;

    try {

        // Stock 1, 1000
        idBuffer_ui = RFID_STOCK1_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_STOCK1_1_ui } : predecessorsBuffer_vec = { RFID_STOCK1_2_ui };
        CCW_i ? successorsBuffer_vec = { RFID_STOCK1_2_ui } : successorsBuffer_vec = { RFID_STOCK1_1_ui };
        typeBuffer_st = RFIDType::START_DESTINATION;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Stock 1, 1001
        idBuffer_ui = RFID_STOCK1_1_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_ASSEMBLY1_4_ui } : predecessorsBuffer_vec = { RFID_STOCK1_2_ui, RFID_STOCK1_ui};
        CCW_i ? successorsBuffer_vec= { RFID_STOCK1_2_ui, RFID_STOCK1_ui } : successorsBuffer_vec = { RFID_ASSEMBLY1_4_ui };
        CCW_i ? typeBuffer_st = RFIDType::DESCENT : typeBuffer_st = RFIDType::GATEWAY;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Stock 1, 1002
        idBuffer_ui = RFID_STOCK1_2_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_STOCK1_1_ui, RFID_STOCK1_ui } : predecessorsBuffer_vec = { RFID_STOCK2_1_ui };
        CCW_i ? successorsBuffer_vec= { RFID_STOCK2_1_ui } : successorsBuffer_vec = { RFID_STOCK1_1_ui, RFID_STOCK1_ui };
        CCW_i ? typeBuffer_st = RFIDType::GATEWAY : typeBuffer_st = RFIDType::DESCENT;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Stock 2, 2000
        idBuffer_ui = RFID_STOCK2_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_STOCK2_1_ui } : predecessorsBuffer_vec = { RFID_STOCK2_2_ui };
        CCW_i ? successorsBuffer_vec= { RFID_STOCK2_2_ui } : successorsBuffer_vec = { RFID_STOCK2_1_ui };
        typeBuffer_st = RFIDType::START_DESTINATION;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Stock 2, 2001
        idBuffer_ui = RFID_STOCK2_1_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_STOCK1_2_ui } : predecessorsBuffer_vec = { RFID_STOCK2_ui, RFID_STOCK2_2_ui};
        CCW_i ? successorsBuffer_vec= { RFID_STOCK2_ui, RFID_STOCK2_2_ui } : successorsBuffer_vec = { RFID_STOCK1_2_ui };
        CCW_i ? typeBuffer_st = RFIDType::DESCENT: typeBuffer_st = RFIDType::GATEWAY;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Stock 2, 2002
        idBuffer_ui = RFID_STOCK2_2_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_STOCK2_ui, RFID_STOCK2_2_ui } : predecessorsBuffer_vec = { RFID_TESTING_1_ui };
        CCW_i ? successorsBuffer_vec= { RFID_TESTING_1_ui } : successorsBuffer_vec = { RFID_STOCK2_ui, RFID_STOCK2_2_ui };
        CCW_i ? typeBuffer_st = RFIDType::GATEWAY : typeBuffer_st = RFIDType::DESCENT;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Testing, 100
        idBuffer_ui = RFID_TESTING_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_ASSEMBLY1_1_ui } : predecessorsBuffer_vec = { RFID_ASSEMBLY1_4_ui };
        CCW_i ? successorsBuffer_vec= { RFID_ASSEMBLY1_4_ui } : successorsBuffer_vec = { RFID_ASSEMBLY1_1_ui };
        typeBuffer_st = RFIDType::START_DESTINATION;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Testing, 101
        idBuffer_ui = RFID_TESTING_1_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_STOCK2_2_ui } : predecessorsBuffer_vec = { RFID_TESTING_ui, RFID_TESTING_4_ui };
        CCW_i ? successorsBuffer_vec= { RFID_TESTING_ui, RFID_TESTING_4_ui } : successorsBuffer_vec = { RFID_STOCK2_2_ui };
        CCW_i ? typeBuffer_st = RFIDType::DESCENT : typeBuffer_st = RFIDType::GATEWAY;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Testing, 104
        idBuffer_ui = RFID_TESTING_4_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_TESTING_ui,  RFID_TESTING_1_ui } : predecessorsBuffer_vec = { RFID_ASSEMBLY1_1_ui };
        CCW_i ? successorsBuffer_vec= { RFID_ASSEMBLY1_1_ui } : successorsBuffer_vec = { RFID_TESTING_ui,  RFID_TESTING_1_ui };
        CCW_i ? typeBuffer_st = RFIDType::GATEWAY : typeBuffer_st = RFIDType::DESCENT;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Assembly 1, 10
        idBuffer_ui = RFID_ASSEMBLY1_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_ASSEMBLY1_1_ui } : predecessorsBuffer_vec = { RFID_ASSEMBLY1_2_ui };
        CCW_i ? successorsBuffer_vec= { RFID_ASSEMBLY1_2_ui } : successorsBuffer_vec = { RFID_ASSEMBLY1_1_ui };
        typeBuffer_st = RFIDType::START_DESTINATION;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Assembly 1, 11
        idBuffer_ui = RFID_ASSEMBLY1_1_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_TESTING_4_ui } : predecessorsBuffer_vec = { RFID_ASSEMBLY1_ui, RFID_ASSEMBLY1_4_ui };
        CCW_i ? successorsBuffer_vec= { RFID_ASSEMBLY1_ui, RFID_ASSEMBLY1_4_ui } : successorsBuffer_vec = { RFID_TESTING_4_ui };
        CCW_i ? typeBuffer_st = RFIDType::DESCENT : typeBuffer_st = RFIDType::GATEWAY;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Assembly 1, 14
        idBuffer_ui = RFID_ASSEMBLY1_4_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_ASSEMBLY1_ui, RFID_ASSEMBLY1_1_ui } : predecessorsBuffer_vec = { RFID_ASSEMBLY2_1_ui };
        CCW_i ? successorsBuffer_vec= { RFID_ASSEMBLY2_1_ui } : successorsBuffer_vec = { RFID_ASSEMBLY1_ui, RFID_ASSEMBLY1_1_ui };
        CCW_i ? typeBuffer_st = RFIDType::GATEWAY : typeBuffer_st = RFIDType::DESCENT;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Assembly 2, 20
        idBuffer_ui = RFID_ASSEMBLY2_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_ASSEMBLY2_1_ui } : predecessorsBuffer_vec = { RFID_ASSEMBLY2_2_ui };
        CCW_i ? successorsBuffer_vec= { RFID_ASSEMBLY2_2_ui } : successorsBuffer_vec = { RFID_ASSEMBLY2_1_ui };
        typeBuffer_st = RFIDType::START_DESTINATION;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Assembly 2, 21
        idBuffer_ui = RFID_ASSEMBLY2_1_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_ASSEMBLY1_4_ui } : predecessorsBuffer_vec = { RFID_ASSEMBLY2_ui, RFID_ASSEMBLY2_4_ui };
        CCW_i ? successorsBuffer_vec= { RFID_ASSEMBLY2_ui, RFID_ASSEMBLY2_4_ui } : successorsBuffer_vec = { RFID_ASSEMBLY1_4_ui };
        CCW_i ? typeBuffer_st = RFIDType::DESCENT : typeBuffer_st = RFIDType::GATEWAY;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

        // Assembly 2, 24
        idBuffer_ui = RFID_ASSEMBLY2_4_ui;
        keyValue_ui = idBuffer_ui;
        CCW_i ? predecessorsBuffer_vec = { RFID_ASSEMBLY2_ui, RFID_ASSEMBLY2_1_ui } : predecessorsBuffer_vec = { RFID_STOCK1_1_ui };
        CCW_i ? successorsBuffer_vec= { RFID_STOCK1_1_ui } : successorsBuffer_vec = { RFID_ASSEMBLY2_ui, RFID_ASSEMBLY2_1_ui };
        CCW_i ? typeBuffer_st = RFIDType::GATEWAY : typeBuffer_st = RFIDType::DESCENT;
        RFIDMap_mp[keyValue_ui] = RFID { idBuffer_ui, predecessorsBuffer_vec, successorsBuffer_vec, typeBuffer_st }; // map all information as a RFID object
        predecessorsBuffer_vec.clear(); // clear for the next tag
        successorsBuffer_vec.clear(); // clear for the next tag

    }  catch (const exception &e) {

        ROS_ERROR("[%s]: Expetion (%s) thrown while generating the map representation.", ros::this_node::getName().c_str(),
                  e.what());

        return false;

    }

    ROS_INFO("[%s]: built map representation successfully...", ros::this_node::getName().c_str());

    return true;

}

/// checkRFID_b
/// \param currentRFID_ui
/// \return true if succeed else false
bool checkRFID_b(uint64_t currentRFID_ui) {

    if (!RFIDMap_mp.empty()) {

        if (RFIDMap_mp.count(currentRFID_ui) <= 0) { // check if the current RFID is registered in the map representation RFIDMap_mp

            return false;

        }

    }

    return true;

}

/// getRFIDAction_b
/// \param initialRFID_ui
/// \param currentRFID_ui
/// \param targetRFID_ui
/// \param lastRFID_ui
/// \param res_c
/// \return true if succeed else false
bool getRFIDAction_b(uint64_t initialRFID_ui, uint64_t currentRFID_ui, uint64_t targetRFID_ui, uint64_t lastRFID_ui,
                     aal_potential_field_navigation::check_rfid::Response &res_c) {

    RFID currentRfidBuffer_c;
    RFID initialRfidBuffer_c;
    RFID lastRFIDBuffer_c;

    currentRFID_ui != DEFAULT_RFID_i ? currentRfidBuffer_c = RFIDMap_mp.find(currentRFID_ui)->second : currentRfidBuffer_c = DEFAULT_RFID_s; // get the RFID object by the current RFID
    initialRFID_ui != DEFAULT_RFID_i ? initialRfidBuffer_c = RFIDMap_mp.find(initialRFID_ui)->second : initialRfidBuffer_c = DEFAULT_RFID_s; // get the RFID object by the initial RFID

    res_c.action_i = RFIDAction::NOTHING; // set the initial value as precaution

    if (currentRfidBuffer_c.type == RFIDType::START_DESTINATION) { // check start/stop case (Robot on initial or target RFID)

        if (currentRfidBuffer_c.id == targetRFID_ui && lastRFID_ui != DEFAULT_RFID_i) {

            res_c.action_i = RFIDAction::STOP;

        } else if (currentRfidBuffer_c.id == initialRfidBuffer_c.id) {

            res_c.action_i = RFIDAction::START;

        }

        return true;

    } else if (currentRfidBuffer_c.type == RFIDType::GATEWAY) { // check ascend case (Robot on ascend RFID)

        res_c.action_i = RFIDAction::ASCEND;

        return true;

    } else if (currentRfidBuffer_c.type == RFIDType::DESCENT) { // check descend case (Robot on descend RFID)

        std::vector<uint64_t> successors_vec = currentRfidBuffer_c.successors;

        for (auto it = successors_vec.begin(); it != successors_vec.end(); ++it) {

            if (*it == targetRFID_ui) { // check if the current RFID is _THE_ descent RFID

                res_c.action_i = RFIDAction::DESCEND;

                return true; // early exit

            }

        }

        res_c.action_i = RFIDAction::TRANSIT; // else transit

    }

    return true; // "es_c.action_i = RFIDAction::NOTHING" is still valid

}

/// ROS SERVICE CALLBACKS

/// getRFIDTypeService_b
/// \param req_c
/// \param res_c
/// \return true if succeed else false
bool getRFIDTypeService_b(aal_potential_field_navigation::get_rfid_type::Request &req_c,
                        aal_potential_field_navigation::get_rfid_type::Response &res_c) {

    ROS_INFO("[%s]: getRFIDTypeService_b is called ...", ros::this_node::getName().c_str());

    res_c.jobDone_i = false; // as a precaution
    res_c.RFIDType_i = DEFAULT_RFID_s.type; // as a precaution

    // get the request fields
    uint64_t currentRFID_ui = req_c.currentRFID_ui;

    if (!mapRoutineDone_b) { // check if the initial map routine is already done

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::MAP_ROUTINE_FLAG_ERROR); // concretize the failure

        return false;

    }

    ROS_INFO("[%s]: getRFIDTypeService_b/checkRFID_b() ...", ros::this_node::getName().c_str());

    if (!checkRFID_b(currentRFID_ui)) { // check if the current and the initial RFID exists

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::CHECK_RFID_ERROR); // concretize the failure

        return false;

    }

    res_c.RFIDType_i = (RFIDMap_mp.find(currentRFID_ui)->second).type; // get the RFID type

    ROS_INFO("[%s]: getRFIDTypeService_b successfully finished ...", ros::this_node::getName().c_str());

    res_c.errorCode_i = ErrorCode::NO_ERROR; // no error occured
    res_c.jobDone_i = true; // quit the job successfully

    return true;

}

/// checkRFIDService_b
/// \param req_c
/// \param res_c
/// \return true if succeed else false
bool checkRFIDService_b(aal_potential_field_navigation::check_rfid::Request &req_c,
                                 aal_potential_field_navigation::check_rfid::Response &res_c) {

    ROS_INFO("[%s]: checkRFIDService_b is called ...", ros::this_node::getName().c_str());

    res_c.jobDone_i = false; // as a precaution
    res_c.located_i = false; // as a precaution
    res_c.action_i = RFIDAction::NOTHING; // default value

    // get the request fields
    uint64_t currentRFID_ui = req_c.currentRFID_ui;
    uint64_t initialRFID_ui = req_c.initialRFID_ui;
    uint64_t targetRFID_ui = req_c.targetRFID_ui;
    uint64_t lastRFID_ui = req_c.lastRFID_ui;

    if (!mapRoutineDone_b) { // check if the initial map routine is already done

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::MAP_ROUTINE_FLAG_ERROR); // concretize the failure

        return false;

    }

    ROS_INFO("[%s]: checkRFIDService_b/checkRFID_b() ...", ros::this_node::getName().c_str());

    if (!checkRFID_b(currentRFID_ui)) { // check if the current and the initial RFID exists

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::CHECK_RFID_ERROR); // concretize the failure

        return false;

    }

    if (!getRFIDAction_b(initialRFID_ui, currentRFID_ui, targetRFID_ui, lastRFID_ui, res_c)) { // get the required action according to the current and the target RFID

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::GET_RFID_ACTION_ERROR); // concretize the failure

        return false;

    }

    ROS_INFO("[%s]: checkRFIDService_b successfully finished ...", ros::this_node::getName().c_str());

//    ROS_INFO("[%s]: res_c.action_i = %i", ros::this_node::getName().c_str(), res_c.action_i); // Debug only!

    res_c.errorCode_i = ErrorCode::NO_ERROR; // no error occured
    res_c.located_i = true; // response that the current RFID is registered
    res_c.jobDone_i = true; // quit the job successfully

    return true;

}

/// MAIN

/// main
/// \param argc_i
/// \param argv_pch
/// \return 0
int main(int argc_i, char *argv_pch[]) {

    // init ROS
    ros::init(argc_i, argv_pch, ros::this_node::getName());
    ros::NodeHandle node_("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());
    ros::Rate loopRate_(LOOP_RATE_i);

    // ROS service advertise
    ros::ServiceServer checkRFIDServiceServer_c = node_.advertiseService(CHECK_RFID_st, checkRFIDService_b);
    ros::ServiceServer getRFIDTypeServiceServer_c = node_.advertiseService(GET_RFID_TYPE_st, getRFIDTypeService_b);

    // initialize the map representation
    if (!initializeRFIDMap_b()) {

        ROS_ERROR("[%s]: initializeRFIDMap_b returns false (= fatal error), shutting node down ...", ros::this_node::getName().c_str());

        return 0;

    } else {

        mapRoutineDone_b = true; // set a global flag if the map routine finish correctly

    }

    ros::spin(); // keep this node alive

    return 0;
}