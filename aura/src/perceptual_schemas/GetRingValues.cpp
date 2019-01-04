/**
 * ==============================================================================
 * \file GetRingValues.cpp
 * \brief Provides the definition of the perceptual schema class GetRingValues
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

#include "aal_potential_field_navigation/perceptual_schemas/GetRingValues.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

int proximityRingIdx_i;

/// LOCAL FUNCTIONS

/// PUBLIC METHODS

/**
 * Constructor
 * -----------------------------------------------------------------
 * additional information are given in the GetRingValues.h
 * -----------------------------------------------------------------
*/
GetRingValues::GetRingValues(string topic_st) : PerceptualSchema(topic_st),
                                                proximityRingValues_aui({numeric_limits<uint16_t>::max()}) {

#if TWB == 1
    this->subProximityRing_c = this->nh_c.subscribe<sai_msgs::Int32MultiArrayStamped>(this->topic_st, 1,
                                                                                         &GetRingValues::proximityRingCallback_v,
                                                                                         this); // set up the subscriber and bind the "proximityRingCallback_v" to the "topic"
#elif TWB == 0
    this->subProximityRing_c = this->nh_c.subscribe<amiro_msgs::UInt16MultiArrayStamped>(this->topic_st, 1,
                                                                                         &GetRingValues::proximityRingCallback_v,
                                                                                         this); // set up the subscriber and bind the "proximityRingCallback_v" to the "topic"
#endif
}

/**
 * Destructor
 * -----------------------------------------------------------------
 * additional information are given in the GetRingValues.h
 * -----------------------------------------------------------------
*/
GetRingValues::~GetRingValues() {

}

/**
 * Return the specific perceptual data
 * ---------------------------------------------------------
 * additional information are given in the GetMinDistance.h
 * ---------------------------------------------------------
*/
uint16_t GetRingValues::getPerceptualData_c(RingSensor ringSensor_e) {

    return this->proximityRingValues_aui[(uint)ringSensor_e];

}

/// PROTECTED METHODS

/// PRIVATE METHODS

/**
 * Callback function which is dedicated to the perceptual
 * schema specific topic
 * ---------------------------------------------------------
 * additional information are given in the GetMinDistance.h
 * ---------------------------------------------------------
*/
#if TWB == 1
void GetRingValues::proximityRingCallback_v(const sai_msgs::Int32MultiArrayStamped::ConstPtr& msg_c) {
#elif TWB == 0
void GetRingValues::proximityRingCallback_v(const amiro_msgs::UInt16MultiArrayStamped::ConstPtr& msg_c) {
#endif

    proximityRingIdx_i = 0;

#if TWB == 1
    for (auto it = msg_c->data.data.begin(); it != msg_c->data.data.end(); it++, proximityRingIdx_i++) {
#elif TWB == 0
    for (auto it = msg_c->array.data.begin(); it != msg_c->array.data.end(); it++, proximityRingIdx_i++) {
#endif

        if (proximityRingIdx_i < PROXIMITY_SENSORS_CNT_i) { // check the array index

//            ROS_INFO("[%s] proximityRingValues_aui[%i] = %i", ros::this_node::getName().c_str(),
//                     proximityRingIdx_i, (uint16_t) (*it)); // Debug only!

            this->proximityRingValues_aui[proximityRingIdx_i] = (uint16_t) (*it);

        } else {

            continue;

        }

    }

}