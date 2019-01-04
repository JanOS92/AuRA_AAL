/**
 * ==============================================================================
 * \file  Constants.h
 * \brief Provides package wide utility functions and includes
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef TWB
#define TWB 0 // TWB/Gazebo flag!
#endif

#if TWB == 0
#define AAL 0 // AAL/Automatica flag!
#endif

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_CONSTANTS_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_CONSTANTS_H

/// INCLUDES

// ROS
#include <nav_msgs/Odometry.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// std library
#include <string>
#include <vector>
#include <limits>

// Types
#include "aal_potential_field_navigation/Types.h"

/// CONSTANTS

// Common
const std::string PROJECT_FOLDER_st = "src/amiro_potential_field_navigation/aal_potential_field_navigation/";

// Mathematical
const float PI_f = 3.1416;
const float PI_HALF_f = PI_f / 2.0f;

// AMiRo
const std::string AMIRO_NAME_st = "/amiro7";
const int PROXIMITY_SENSORS_CNT_i = 8;
const int FLOOR_SENSORS_CNT_i = 4;

const std::vector<ProximityRingSensor> proximityRing_vec = {ProximityRingSensor{PROX_BACK_LEFT, 2.7489},
                                                            ProximityRingSensor{PROX_LEFT_BACK, 1.9635},
                                                            ProximityRingSensor{PROX_LEFT_FRONT, 1.178},
                                                            ProximityRingSensor{PROX_FRONT_LEFT, 0.3927},
                                                            ProximityRingSensor{PROX_FRONT_RIGHT, 5.8905},
                                                            ProximityRingSensor{PROX_RIGHT_FRONT, 5.1051},
                                                            ProximityRingSensor{PROX_RIGHT_BACK, 4.3197},
                                                            ProximityRingSensor{PROX_BACK_RIGHT, 3.5343}};

// Scenario
const int CCW_i = 1;
const bool INVERT_DIRECTION_b = false;
const int PIXEL_MODE_i = 0;
const double PIXEL_SCALE_d = 1.0;

#if TWB == 0 // Gazebo

    const int BLACK_LINE_i = 10000;
    const uint16_t MINIMUM_DISTANCE_ui = 20000;
    const double MAX_MOTION_VEC_ABS_d = 1.5;
    const double MAX_MOTION_VEC_ROT_d = PI_HALF_f;
    const double TRANS_VEL_SCALE_MS_d = 0.05;
    const double ROT_VEL_SCALE_RS_d = 1.0;

    // AMiRo
    const uint16_t PROX_RING_SEN_VAL_OFF_ui = 14000;
    const uint16_t MAX_PROX_RING_SEN_VAL_ui = std::numeric_limits<uint16_t>::max();

#if AAL == 1 // AMiRo Assembly Line

    // image directory
    const std::string MEDIA_DIRECTORY_PATH_st = PROJECT_FOLDER_st + "media/";
    const std::string IMAGE_DIRECTORY_PATH_st = PROJECT_FOLDER_st + "aal/";

    // image names
    const std::string MAP_IMAGE_NAME_st = "assembly_line";
    const std::string SECONDARY_LINE_1_IMAGE_NAME_st = "secondary_line_1";
    const std::string SECONDARY_LINE_2_IMAGE_NAME_st = "secondary_line_2";
    const std::string SECONDARY_LINE_3_IMAGE_NAME_st = "secondary_line_3";
    const std::string TRANSIT_IMAGE_NAME_st = "transit";

    // image size
    const int IMAGE_HEIGHT_i = 2000;
    const int IMAGE_WIDTH_i = 2000;
    const double METER_PER_PIXEL_WIDTH_d = 0.0025;
    const double METER_PER_PIXEL_HEIGHT_d = 0.0025;

#elif AAL == 0 // Automatica

    // image directory
    const std::string MEDIA_DIRECTORY_PATH_st = PROJECT_FOLDER_st + "media/";
    const std::string IMAGE_DIRECTORY_PATH_st = PROJECT_FOLDER_st + "media/automatica_marker";

    // image names
    const std::string namePrefix_st = "Automatica_";
    const std::string MAP_IMAGE_NAME_st = namePrefix_st + "Blank_marker";
    const std::string SECONDARY_LINE_1_IMAGE_NAME_st = namePrefix_st + "SecondaryLine1_marker";
    const std::string SECONDARY_LINE_2_IMAGE_NAME_st = namePrefix_st + "SecondaryLine2_marker";
    const std::string SECONDARY_LINE_3_IMAGE_NAME_st = namePrefix_st + "SecondaryLine3_marker";
    const std::string TRANSIT_IMAGE_NAME_st = namePrefix_st + "Transit_marker";

    // image size
    const int IMAGE_WIDTH_i = 773;
    const int IMAGE_HEIGHT_i = 408;
    const double METER_PER_PIXEL_WIDTH_d = 0.0022; // measured (± 1cm)
    const double METER_PER_PIXEL_HEIGHT_d = 0.0022; // measured (± 1cm)

#endif

    // ROS Topics: Communication
    const std::string ODOM_AMIRO_TOPIC_st = AMIRO_NAME_st + "/odom";

    // potentialfield processing
    const float HEURISTIC_FACTOR_CURRENT_f = 2.5f;
    const float HEURISTIC_ABS_MIN_CURRENT_f = 1.0f;
    const float ALPHA_f = 1.0f / 200.0f;

    // image preprocessing
    const int KERNEL_SIZE_DILATE_WMA_X_i = 100; // WMA: white map alignment
    const int KERNEL_SIZE_DILATE_WMA_Y_i = 100; // WMA: white map alignment
    const int KERNEL_SIZE_DILATE_LD_X_i = 5; // LD: line direction
    const int KERNEL_SIZE_DILATE_LD_Y_i = 5; // LD: line direction
    const int KERNEL_SIZE_ERODE_CL_X_i = 2; // CL: center line
    const int KERNEL_SIZE_ERODE_CL_Y_i = 2; // CL: center line

#elif TWB == 1 // TWB

    const int BLACK_LINE_i = 10000;
    const uint16_t MINIMUM_DISTANCE_ui = 100;
    const double MAX_MOTION_VEC_ABS_d = 0.75;
    const double MAX_MOTION_VEC_ROT_d = PI_HALF_f;
    const double TRANS_VEL_SCALE_MS_d = 0.05;
    const double ROT_VEL_SCALE_RS_d = 1.0;
    const double METER_PER_PIXEL_WIDTH_d = 0.002175; // measured (± 1cm)
    const double METER_PER_PIXEL_HEIGHT_d = 0.002195; // measured (± 1cm)

    // AMiRo
    const uint16_t PROX_RING_SEN_VAL_OFF_ui = 0;
    const uint16_t MAX_PROX_RING_SEN_VAL_ui = 750;

    // image size
    const int IMAGE_WIDTH_i = 773;
    const int IMAGE_HEIGHT_i = 408;

    // image directory
    const std::string MEDIA_DIRECTORY_PATH_st = PROJECT_FOLDER_st + "media/";
    const std::string IMAGE_DIRECTORY_PATH_st = PROJECT_FOLDER_st + "/media/automatica_marker";

    // image names
    const std::string namePrefix_st = "Automatica_";
    const std::string MAP_IMAGE_NAME_st = namePrefix_st + "Blank_marker";
    const std::string SECONDARY_LINE_1_IMAGE_NAME_st = namePrefix_st + "SecondaryLine1_marker";
    const std::string SECONDARY_LINE_2_IMAGE_NAME_st = namePrefix_st + "SecondaryLine2_marker";
    const std::string SECONDARY_LINE_3_IMAGE_NAME_st = namePrefix_st + "SecondaryLine3_marker";
    const std::string TRANSIT_IMAGE_NAME_st = namePrefix_st + "Transit_marker";
    const std::string TRANSIT_IMAGE_NAME_st = namePrefix_st + "edit";

    // ROS Topics: Communication
    const std::string ODOM_AMIRO_TOPIC_st = "/aruco3_node3_2/cam/odom/5";

    // potentialfield processing
    const float HEURISTIC_FACTOR_CURRENT_f = 2.5f;
    const float HEURISTIC_ABS_MIN_CURRENT_f = 1.0f;
    const float ALPHA_f = 1.0f / 250.0f;

    // image preprocessing
    const int KERNEL_SIZE_DILATE_WMA_X_i = 100; // WMA: white map alignment
    const int KERNEL_SIZE_DILATE_WMA_Y_i = 100; // WMA: white map alignment
    const int KERNEL_SIZE_DILATE_LD_X_i = 7; // LD: line direction
    const int KERNEL_SIZE_DILATE_LD_Y_i = 7; // LD: line direction
    const int KERNEL_SIZE_ERODE_CL_X_i = 2; // CL: center line
    const int KERNEL_SIZE_ERODE_CL_Y_i = 2; // CL: center line

#else

    #error "The TWB flag is not defined!"

#endif

// Motor schema names
const std::string MOTOR_SCHEMA_DEFAULT_NAME_str = "";
const std::string MOTOR_SCHEMA_MOVE_TO_TARGET_NAME_str = "move-to-target";
const std::string MOTOR_SCHEMA_MOVE_TO_GOAL_NAME_str = "move-to-goal";
const std::string MOTOR_SCHEMA_STAY_ON_PATH_NAME_str = "stay-on-path";
const std::string MOTOR_SCHEMA_AVOID_OBSTACLE_NAME_str = "avoid-obstacle";
const std::string MOTOR_SCHEMA_WAIT_FOR_OBSTACLE_NAME_str = "wait-for-obstacle";
const std::string MOTOR_SCHEMA_MOVE_ROBOT_NAME_str = "move-robot";
const std::vector<std::string> MOTOR_SCHEMA_NAMES_vec = {MOTOR_SCHEMA_MOVE_TO_TARGET_NAME_str,
                                                         MOTOR_SCHEMA_MOVE_TO_GOAL_NAME_str,
                                                         MOTOR_SCHEMA_STAY_ON_PATH_NAME_str,
                                                         MOTOR_SCHEMA_AVOID_OBSTACLE_NAME_str,
                                                         MOTOR_SCHEMA_WAIT_FOR_OBSTACLE_NAME_str,
                                                         MOTOR_SCHEMA_MOVE_ROBOT_NAME_str};

// ROS
const int32_t MAX_NS_TO_LISTEN_i = 40e6; // ns (causes 25 Hz)
const int64_t LOOP_RATE_i = 100; // Hz
const int32_t MAX_NS_TO_WAIT_FOR_SUB_i = 2; // s
const ros::Duration TIME_TO_WAIT_OF_STP_c(10.0); // s

// ROS Topics: Interconnection
const std::string DEFAULT_TOPIC_st = "/";
const std::string RAW_IMAGE_TOPIC_st = "/image";
const std::string MOTION_VEC_TOPIC_st = AMIRO_NAME_st + "/motion_vec";
const std::string FINAL_GRADIENT_TOPIC_st = AMIRO_NAME_st + "/final_gradient";
const std::string VEC_FIELD_DIR_TOPIC_st = "/vectorfield/direction_image";
const std::string VEC_FIELD_ALIGN_TOPIC_st = "/vectorfield/alignment_image";
const std::string VEC_FIELD_FINAL_TOPIC_st = "/vectorfield/final_image";
const std::string GRIDMAP_TOPIC_st = "/vectorfield/image/gridmap";
const std::string RGB_TOPIC_st = GRIDMAP_TOPIC_st + "/rgb_image";
const std::string VECTORFIELD_LINK_TOPIC_st = "potential_field";
const std::string COOP_TOPIC_st = AMIRO_NAME_st + "motor_schema_cooperation";

// ROS Topics: Communication
const std::string PROXIMITY_RING_TOPIC_st = AMIRO_NAME_st + "/proximity_ring/values/";
const std::string PROXIMITY_FLOOR_TOPIC_st = AMIRO_NAME_st + "/proximity_floor/values/";
const std::string TWIST_TOPIC_st = AMIRO_NAME_st + "/cmd_vel";
const std::string VECTOR_TOPIC_st = AMIRO_NAME_st + "/vector";
const std::string RFID_TAG_TOPIC_st = AMIRO_NAME_st + "/rfid_tag_list";
const std::string ODOM_MARKER_TOPIC_st = "/aruco3_node3_2/cam/odom/0";

// Frame Ids
const std::string FRAME_POT_FIELD_st = "potentialfield";
const std::string FRAME_MAP_st = "map";
const std::string FRAME_CAMERA_st = "cam3";
const std::string FRAME_AMIRO_BASE_LINK_st = AMIRO_NAME_st + "/base_link";

// ROS Service
const std::string INITIATE_IMAGE_ROUTINE_st = "initiate_image_routine";
const std::string GET_FIELD_BY_KEY_st = "/initiate_image_routine/get_field_by_key";
const std::string CHECK_RFID_st = "/map_representation_node/check_rfid";
const std::string GET_RFID_TYPE_st = "/map_representation_node/get_rfid_type";

// ROS Action
const std::string MOVE_TO_TARGET_st = "call_move_to_target";
const std::string MOVE_TO_GOAL_st = "call_move_to_goal";
const std::string MOVE_ROBOT_st = "call_move_robot";
const std::string STAY_ON_PATH_st = "call_stay_on_path";
const std::string WAIT_FOR_OBSTACLE_st = "call_wait_for_obstacle";
const std::string AVOID_OBSTACLE_st = "call_avoid_obstacle";
const std::string TRACE_st = "call_trace";

// ROS Topics: Visualization
const std::string FINAL_GRADIENT_MARKER_TOPIC_st = "visualization_marker_c";
const std::string MAP_IMAGE_TOPIC_st = "map_image";
const std::string PLOT_ORIENTATION_TOPIC_st = "plot_orientation";

// edge clustering
const uint8_t THRESHOLD_VALUE_ui = 100;
const uint8_t MAX_BINARY_VALUE_ui = 255;

// vectorfield types
const std::string CURRENT_st = "current";
const std::string CHARGE_st = "charge";

// image names
const int IMAGE_NAMES_CNT_i = 4;
const std::vector<std::string> IMAGE_NAMES_vec = {SECONDARY_LINE_1_IMAGE_NAME_st, SECONDARY_LINE_2_IMAGE_NAME_st,
                                                  SECONDARY_LINE_3_IMAGE_NAME_st, TRANSIT_IMAGE_NAME_st};

// image size
const cv::Size IMAGE_SIZE_sz(IMAGE_WIDTH_i, IMAGE_HEIGHT_i);

// image directory
const std::string IMAGE_TYPE_st = ".png";

// image preprocessing
const int FIRST_CHILD_CONTOUR_IDX_i = 2;
const int BLOCK_SIZE_HARRIS_i = 2;
const int APERTURE_SIZE_HARRIS_i = 3;
const double HARRIS_PARAMETER_d = 0.04;
const int THRESHOLD_HARRIS_i = 225;
const int MAX_THRESHOLD_HARRIS_i = 255;
const int LOW_THRESHOLD_CANNY_i = 0;
const int RATIO_CANNY_i = 3;
const int KERNEL_SIZE_CANNY_i = 3;
const int LINE_THICKNESS_i = 8;
const int SIDE_LENGTH_i = 2;
const int DYE_VALUE_HALF_i = 128;
const int DYE_VALUE_MAX_i = 255;
const cv::Vec3b GRAY_vec = cv::Vec3b(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i);
const cv::Vec3b BLACK_vec = cv::Vec3b(0.0, 0.0, 0.0);
const cv::Vec3b RED_vec = cv::Vec3b(0.0, 0.0, DYE_VALUE_MAX_i);
const cv::Vec3b BLUE_vec = cv::Vec3b(DYE_VALUE_MAX_i, 0.0, 0.0);
const cv::Vec3b WHITE_vec = cv::Vec3b(DYE_VALUE_MAX_i, DYE_VALUE_MAX_i, DYE_VALUE_MAX_i);
const int BACKGROUND_IDX_CONNECTED_COMPONENT_i = 0;
const int OUTTER_LINE_IDX_CONNECTED_COMPONENT_i = 1;
const int INNER_LINE_IDX_CONNECTED_COMPONENT_i = 2;
const int EXPECTED_LABEL_NUMBER_i = 3;
const int KERNEL_SIZE_GAUSS_BLUR_i = 7;
const std::string IMAGE_TYPE_CURRENT_st = "current";
const std::string IMAGE_TYPE_CHARGE_st = "charge";
const int DESIRED_VECTORFIELD_HEIGTH_i = IMAGE_HEIGHT_i;
const int DESIRED_VECTORFIELD_WIDTH_i = IMAGE_WIDTH_i;
const cv::Point DILATE_CENTER_POINT_c = cv::Point(-1, -1); // cv::Point(-1, -1) means that the anchor is at the element center.
const int DILATE_ITERATIONS_i = 2;
const int DILATE_BORDER_TYPE_i = cv::BORDER_REPLICATE;

// potentialfield processing
const std::string FIELD_TYPE_KEYWORD_POT_st = "potentialField";
const std::string FIELD_TYPE_KEYWORD_VEC_st = "vectorField";
const std::string FIELD_NAME_POSTFIX_DIR_st = "direction";
const std::string FIELD_NAME_POSTFIX_ALIGN_st = "alignment";
const std::string DEFAULT_FIELD_KEY_st = "";

// default values
const int DEFAULT_RFID_i = 0;
const vector<uint64_t> DEFAULT_RFID_LIST_vec = {};
const RFID DEFAULT_RFID_s = {DEFAULT_RFID_i, DEFAULT_RFID_LIST_vec, DEFAULT_RFID_LIST_vec, RFIDType::DEFAULT};
const cv::Mat DEFAULT_VECTORFIELD_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f));
const float DEFAULT_POS_f = 0.0;
const float DEFAULT_EUL_YAW_f = 0.0;
const cv::Point2i DEFAULT_POINT_c = cv::Point2i((int) DEFAULT_POS_f, (int) DEFAULT_POS_f);
const int DEFAULT_MOTION_VEC_PRIO_i = 0;
const MotionVector DEFAULT_MOTION_VEC_s = {0.0f, 0.0f, MotionVecMsgType::ILLEGAL, DEFAULT_MOTION_VEC_PRIO_i,
                                           MOTOR_SCHEMA_DEFAULT_NAME_str};
const uint16_t DEFAULT_PROXIMITY_SENSOR_VALUE_ui = std::numeric_limits<uint16_t>::max();
const Pose DEFAULT_POSE_c = {0.0, 0.0, 0.0, 0.0};

// RFID Tags
const uint64_t RFID_STOCK1_ui = 1000;
const uint64_t RFID_STOCK1_1_ui = 1001;
const uint64_t RFID_STOCK1_2_ui = 1003;
const uint64_t RFID_STOCK2_ui = 2000;
const uint64_t RFID_STOCK2_1_ui = 2001;
const uint64_t RFID_STOCK2_2_ui = 2002;
const uint64_t RFID_TESTING_ui = 100;
const uint64_t RFID_TESTING_1_ui = 101;
const uint64_t RFID_TESTING_2_ui = 102;
const uint64_t RFID_TESTING_3_ui = 103;
const uint64_t RFID_TESTING_4_ui = 104;
const uint64_t RFID_ASSEMBLY1_ui = 10;
const uint64_t RFID_ASSEMBLY1_1_ui = 11;
const uint64_t RFID_ASSEMBLY1_2_ui = 12;
const uint64_t RFID_ASSEMBLY1_3_ui = 13;
const uint64_t RFID_ASSEMBLY1_4_ui = 14;
const uint64_t RFID_ASSEMBLY2_ui = 20;
const uint64_t RFID_ASSEMBLY2_1_ui = 21;
const uint64_t RFID_ASSEMBLY2_2_ui = 22;
const uint64_t RFID_ASSEMBLY2_3_ui = 23;
const uint64_t RFID_ASSEMBLY2_4_ui = 24;

// Gridmap
const std::string ELEVATION_str = "elevation";
const std::string COLOR_str = "color";

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_CONSTANTS_H
