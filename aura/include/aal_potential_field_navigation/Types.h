/**
 * ==============================================================================
 * \file Types.h
 * \brief Provides package wide custom types
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_TYPES_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_TYPES_H

#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <vector>

using namespace std;

enum ErrorCode {

    NO_ERROR = 0,
    LOAD_IMAGE_ERROR,
    REDYE_IMAGE_ERROR,
    GET_IMAGE_SERVICE_ERROR,
    CONVERT_IMAGE_TO_FIELD_ERROR,
    IMAGE_ROUTINE_FLAG_ERROR,
    MAP_THE_FIELDS_ERROR,
    GET_FIELD_BY_KEY_ERROR,
    BEHAVIOR_EXECUTION_ERROR,
    BEHAVIOR_INITIALIZATION_ERROR,
    MAP_ROUTINE_FLAG_ERROR,
    CHECK_RFID_ERROR,
    GET_RFID_ACTION_ERROR,
    GENERATE_FIELD_KEY_ERROR,
    BEHAVIOR_ERROR,

};

enum RFIDType {

    DEFAULT = 0,
    START_DESTINATION,
    GATEWAY,
    DESCENT

};

enum RFIDAction {

    NOTHING = 0,
    START,
    STOP,
    ASCEND,
    DESCEND,
    TRANSIT

};

enum RotateFlags {

    ROTATE_90_CLOCKWISE = 1,        // rotate 90 degrees clockwise
    ROTATE_90_COUNTERCLOCKWISE = 2, // rotate 270 degrees clockwise
    ROTATE_180 = 3,                 // rotate 180 degrees clockwise

};

enum MotionVecMsgType {

    NONE = 0,
    ILLEGAL,
    COMMON,
    INTERRUPT

};

enum FloorSensor {

    FLOOR_RIGHT_FRONT = 0,
    FLOOR_RIGHT_SIDE,
    FLOOR_LEFT_SIDE,
    FLOOR_LEFT_FRONT

};

enum JobLogic {

    DEFAULT_JOB = 0,
    GOAL_JOB

};

enum RingSensor {

    PROX_BACK_LEFT = 0,
    PROX_LEFT_BACK,
    PROX_LEFT_FRONT,
    PROX_FRONT_LEFT,
    PROX_FRONT_RIGHT,
    PROX_RIGHT_FRONT,
    PROX_RIGHT_BACK,
    PROX_BACK_RIGHT

};

struct Error {

    ErrorCode errorCode;
    uint8_t prio;

};

struct Pose {

    double x;
    double y;
    double z;
    double theta;

};

struct MotionVector {

    float x;                // x coordinate
    float y;                // y coordinate
    MotionVecMsgType type;  // vector type
    int prio;               // vector priority
    std::string sender;     // associated motor schema

};

//typedef struct RFID {
struct RFID {

    uint64_t id;
    std::vector<uint64_t> predecessors;
    std::vector<uint64_t> successors;
    RFIDType type;

};

//typedef struct ProximityRingSensor {
struct ProximityRingSensor {

    int index;
    float phi;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_TYPES_H
