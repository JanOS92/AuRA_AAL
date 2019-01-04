/**
 * ==============================================================================
 * \file GetFloorValues.cpp
 * \brief Provides the definition of the perceptual schema class GetFloorValues
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/perceptual_schemas/GetFloorValues.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

int proximityFloorIdx_i;

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * ---------------------------------------------------------
 * additional information are given in the GetFloorValues.h
 * ---------------------------------------------------------
*/
GetFloorValues::GetFloorValues(string topic_st) : PerceptualSchema(topic_st) {

#if TWB == 1
    this->subFloorSensorValues_c = this->nh_c.subscribe<sai_msgs::Int32MultiArrayStamped>(this->topic_st, 1,
                                                                                             &GetFloorValues::floorSensorCallback_v,
                                                                                             this); // set up the subscriber and bind the "floorSensorCallback_v" to the "topic"
#elif TWB == 0
    this->subFloorSensorValues_c = this->nh_c.subscribe<amiro_msgs::UInt16MultiArrayStamped>(this->topic_st, 1,
                                                                                             &GetFloorValues::floorSensorCallback_v,
                                                                                             this); // set up the subscriber and bind the "floorSensorCallback_v" to the "topic"
#endif

}

/**
 * Destructor
 * -----------------------------------------------------------------
 * additional information are given in the GetFloorValues.h
 * -----------------------------------------------------------------
*/
GetFloorValues::~GetFloorValues() {

}

/**
 * Return the specific perceptual data
 * ---------------------------------------------------------
 * additional information are given in the GetFloorValues.h
 * ---------------------------------------------------------
*/
uint16_t GetFloorValues::getPerceptualData_ui(FloorSensor floorSensor) {

    if((uint16_t)floorSensor < FLOOR_SENSORS_CNT_i) { // check the array index

        return this->floorSensorValues_aui[floorSensor]; // return the requested floor sensor value

    } else {

        return DEFAULT_PROXIMITY_SENSOR_VALUE_ui;

    }

}

/// PROTECTED METHODS

/// PRIVATE METHODS

/**
 * Callback function which is dedicated to the perceptual
 * schema specific topic
 * ---------------------------------------------------------
 * additional information are given in the GetFloorValues.h
 * ---------------------------------------------------------
*/
#if TWB == 1
void GetFloorValues::floorSensorCallback_v(const sai_msgs::Int32MultiArrayStamped::ConstPtr& msg_c) {
#elif TWB == 0
void GetFloorValues::floorSensorCallback_v(const amiro_msgs::UInt16MultiArrayStamped::ConstPtr& msg_c) {
#endif

    proximityFloorIdx_i = 0;

#if TWB == 1
    for (auto it = msg_c->data.data.begin(); it != msg_c->data.data.end(); it++, proximityFloorIdx_i++) {
#elif TWB == 0
    for (auto it = msg_c->array.data.begin(); it != msg_c->array.data.end(); it++, proximityFloorIdx_i++) {
#endif

        if (proximityFloorIdx_i < FLOOR_SENSORS_CNT_i) { // check the array index

//            ROS_INFO("[%s] floorSensorValues_aui[%i] = %i", ros::this_node::getName().c_str(), proximityFloorIdx_i,
//                     (uint16_t) (*it)); // Debug only!

            this->floorSensorValues_aui[proximityFloorIdx_i] = (uint16_t) (*it);

        } else {

            continue;

        }

    }

}

