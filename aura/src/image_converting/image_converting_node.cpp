/**
 * ==============================================================================
 * @file image_converting_node.cpp
 * @brief Provides an on-demand (ROS service) image loading and publishing
 * @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

// ROS service
#include "../../../../../install/include/aal_potential_field_navigation/initiate_image_routine.h"
#include "../../../../../install/include/aal_potential_field_navigation/get_field_by_key.h"

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Utils and Types
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/utils/utils.h"
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/utils/image_converting_utils.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES
static bool imageRoutineDone_b = false;

string imagePath_st = "";

std::map<string, cv::Mat> fieldMap_mp;

cv::Mat secondaryLine1Gray_mat(IMAGE_SIZE_sz, CV_8UC1, cv::Scalar(uchar(0))),
        secondaryLine2Gray_mat(IMAGE_SIZE_sz, CV_8UC1, cv::Scalar(uchar(0))),
        secondaryLine3Gray_mat(IMAGE_SIZE_sz, CV_8UC1, cv::Scalar(uchar(0))),
        transitGray_mat(IMAGE_SIZE_sz, CV_8UC1, cv::Scalar(uchar(0))); // instantiate as grayscale images of the size "size"

cv::Mat directionSecondaryLine1RedyedDirection_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)),
        directionSecondaryLine2RedyedDirection_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)),
        directionSecondaryLine3RedyedDirection_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)),
        directionTransitRedyedDirection_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)); // instantiate as BGR images of the size "size"

cv::Mat directionSecondaryLine1RedyedAlignment_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)),
        directionSecondaryLine2RedyedAlignment_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)),
        directionSecondaryLine3RedyedAlignment_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)),
        directionTransitRedyedAlignment_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)); // instantiate as BGR images of the size "size"

cv::Mat directionSecondaryLine1VectorFieldDirection_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)),
        directionSecondaryLine2VectorFieldDirection_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)),
        directionSecondaryLine3VectorFieldDirection_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)),
        directionTransitVectorFieldDirection_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)); // instantiate as vectorfield matrices of the size "size"

cv::Mat potentialFieldSecondaryLine1Direction_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)),
        potentialFieldSecondaryLine2Direction_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)),
        potentialFieldSecondaryLine3Direction_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)),
        potentialFieldTransitDirection_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)); // instatiate as matrices of the size "size"

cv::Mat directionSecondaryLine1VectorFieldAlignment_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)),
        directionSecondaryLine2VectorFieldAlignment_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)),
        directionSecondaryLine3VectorFieldAlignment_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)),
        directionTransitVectorFieldAlignment_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f, 0.0f)); // instantiate as vectorfield matrices of the size "size"

cv::Mat potentialFieldSecondaryLine1Alignment_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)),
        potentialFieldSecondaryLine2Alignment_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)),
        potentialFieldSecondaryLine3Alignment_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)),
        potentialFieldTransitAlignment_mat(IMAGE_SIZE_sz, CV_32FC1, cv::Scalar(0.0f)); // instatiate as matrices of the size "size"

/// LOCAL FUNCTIONS

/// getImagePath_st
/// \param imageName_st
/// \return
string getImagePath_st(string imageName_st) {

    string final_image_path_st = IMAGE_DIRECTORY_PATH_st;
    final_image_path_st += "/" + imageName_st;
    final_image_path_st += IMAGE_TYPE_st;

    return final_image_path_st;

}

/// loadImages_b
/// \return true if succeed else false
bool loadImages_b() {

    try {

        secondaryLine1Gray_mat = cv::imread(getImagePath_st(SECONDARY_LINE_1_IMAGE_NAME_st), CV_LOAD_IMAGE_GRAYSCALE); // load image as grayscale
        secondaryLine2Gray_mat = cv::imread(getImagePath_st(SECONDARY_LINE_2_IMAGE_NAME_st), CV_LOAD_IMAGE_GRAYSCALE); // load image as grayscale
        secondaryLine3Gray_mat = cv::imread(getImagePath_st(SECONDARY_LINE_3_IMAGE_NAME_st), CV_LOAD_IMAGE_GRAYSCALE); // load image as grayscale
        transitGray_mat = cv::imread(getImagePath_st(TRANSIT_IMAGE_NAME_st), CV_LOAD_IMAGE_GRAYSCALE); // load image as grayscale

    } catch (const exception &e) {

        ROS_ERROR("[%s]: Expetion (%s) thrown while loading a path sequence.", ros::this_node::getName().c_str(),
                  e.what());

        return false;

    }

    cv::namedWindow( "image", cv::WINDOW_AUTOSIZE); // Debug only!
    cv::imshow( "image", secondaryLine3Gray_mat ); // Debug only!
    cv::waitKey(0); // Debug only!

    return true;

}

/// redyeDirectionImages_b
/// \return true if succeed else false
bool redyeDirectionImages_b() {

    try {

        directionSecondaryLine1RedyedDirection_mat = redyeDirection_mat(secondaryLine1Gray_mat);
        directionSecondaryLine2RedyedDirection_mat = redyeDirection_mat(secondaryLine2Gray_mat);
        directionSecondaryLine3RedyedDirection_mat = redyeDirection_mat(secondaryLine3Gray_mat);
        directionTransitRedyedDirection_mat = redyeDirection_mat(transitGray_mat);

    } catch (const exception &e) {

        ROS_ERROR("[%s]: Expetion (%s) thrown while redying the direction images.", ros::this_node::getName().c_str(),
                  e.what());

        return false;
    }

//    cv::namedWindow( "image", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "image", directionSecondaryLine3RedyedDirection_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return true;

}

/// redyeAlignmentImages_b
/// \return true if succeed else false
bool redyeAlignmentImages_b() {

    try {

        directionSecondaryLine1RedyedAlignment_mat = redyeAlignment_mat(secondaryLine1Gray_mat);
        directionSecondaryLine2RedyedAlignment_mat = redyeAlignment_mat(secondaryLine2Gray_mat);
        directionSecondaryLine3RedyedAlignment_mat = redyeAlignment_mat(secondaryLine3Gray_mat);
        directionTransitRedyedAlignment_mat = redyeAlignment_mat(transitGray_mat);

    } catch (const exception &e) {

        ROS_ERROR("[%s]: Expetion (%s) thrown while redying the alignment images.", ros::this_node::getName().c_str(),
                  e.what());

        return false;

    }

//    cv::namedWindow( "image", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "image", directionSecondaryLine1RedyedAlignment_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return true;

}

/// convertImageToVectorfield_b
/// \return true if succeed else false
bool convertImageToVectorfield_b() {

    std::pair<cv::Mat, cv::Mat> buffer_pair; // create a buffer_pair array

    try {

        // direction
        buffer_pair = imageToVectorfield_pmat(directionSecondaryLine1RedyedDirection_mat, IMAGE_TYPE_CURRENT_st, true); // SecondaryLine1
        directionSecondaryLine1VectorFieldDirection_mat = buffer_pair.first;
        potentialFieldSecondaryLine1Direction_mat = buffer_pair.second;
        buffer_pair = imageToVectorfield_pmat(directionSecondaryLine2RedyedDirection_mat, IMAGE_TYPE_CURRENT_st, true); // SecondaryLine2
        directionSecondaryLine2VectorFieldDirection_mat = buffer_pair.first;
        potentialFieldSecondaryLine2Direction_mat = buffer_pair.second;
        buffer_pair = imageToVectorfield_pmat(directionSecondaryLine3RedyedDirection_mat, IMAGE_TYPE_CURRENT_st, true); // SecondaryLine3
        directionSecondaryLine3VectorFieldDirection_mat = buffer_pair.first;
        potentialFieldSecondaryLine3Direction_mat = buffer_pair.second;
        buffer_pair = imageToVectorfield_pmat(directionTransitRedyedDirection_mat, IMAGE_TYPE_CURRENT_st, true); // Transit
        directionTransitVectorFieldDirection_mat = buffer_pair.first;
        potentialFieldTransitDirection_mat = buffer_pair.second;

        // alignment
        buffer_pair = imageToVectorfield_pmat(directionSecondaryLine1RedyedAlignment_mat, IMAGE_TYPE_CHARGE_st, true); // SecondaryLine1
        directionSecondaryLine1VectorFieldAlignment_mat = buffer_pair.first;
        potentialFieldSecondaryLine1Alignment_mat = buffer_pair.second;
        buffer_pair = imageToVectorfield_pmat(directionSecondaryLine2RedyedAlignment_mat, IMAGE_TYPE_CHARGE_st, true); // SecondaryLine2
        directionSecondaryLine2VectorFieldAlignment_mat = buffer_pair.first;
        potentialFieldSecondaryLine2Alignment_mat = buffer_pair.second;
        buffer_pair = imageToVectorfield_pmat(directionSecondaryLine3RedyedAlignment_mat, IMAGE_TYPE_CHARGE_st, true); // SecondaryLine3
        directionSecondaryLine3VectorFieldAlignment_mat = buffer_pair.first;
        potentialFieldSecondaryLine3Alignment_mat = buffer_pair.second;
        buffer_pair = imageToVectorfield_pmat(directionTransitRedyedAlignment_mat, IMAGE_TYPE_CHARGE_st, true); // Transit
        directionTransitVectorFieldAlignment_mat = buffer_pair.first;
        potentialFieldTransitAlignment_mat = buffer_pair.second;

    } catch (const exception &e) {

        ROS_ERROR("[%s]: Expetion (%s) thrown while converting the images to vector- and potentialfields.", ros::this_node::getName().c_str(),
                  e.what());

        return false;

    }

    return true;
}

/// mapTheFields_b
/// \return true if succeed else false
bool mapTheFields_b() {

    string keyword_st = "";

    try {

        // direction
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + SECONDARY_LINE_1_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // SecondaryLine1, vector field
        fieldMap_mp[keyword_st] = directionSecondaryLine1VectorFieldDirection_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + SECONDARY_LINE_1_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // SecondaryLine1, potential field
        fieldMap_mp[keyword_st] = potentialFieldSecondaryLine1Direction_mat;
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + SECONDARY_LINE_2_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // SecondaryLine2, vector field
        fieldMap_mp[keyword_st] = directionSecondaryLine2VectorFieldDirection_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + SECONDARY_LINE_2_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // SecondaryLine2, potential field
        fieldMap_mp[keyword_st] = potentialFieldSecondaryLine2Direction_mat;
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + SECONDARY_LINE_3_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // SecondaryLine3, vector field
        fieldMap_mp[keyword_st] = directionSecondaryLine3VectorFieldDirection_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + SECONDARY_LINE_3_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // SecondaryLine3, potential field
        fieldMap_mp[keyword_st] = potentialFieldSecondaryLine3Direction_mat;
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + TRANSIT_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // Transit, vector field
        fieldMap_mp[keyword_st] = directionTransitVectorFieldDirection_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + TRANSIT_IMAGE_NAME_st + FIELD_NAME_POSTFIX_DIR_st; // Transit, potential field
        fieldMap_mp[keyword_st] = potentialFieldTransitDirection_mat;

        // alignment
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + SECONDARY_LINE_1_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // SecondaryLine1, vector field
        fieldMap_mp[keyword_st] = directionSecondaryLine1VectorFieldAlignment_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + SECONDARY_LINE_1_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // SecondaryLine1, potential field
        fieldMap_mp[keyword_st] = potentialFieldSecondaryLine1Alignment_mat;
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + SECONDARY_LINE_2_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // SecondaryLine2, vector field
        fieldMap_mp[keyword_st] = directionSecondaryLine2VectorFieldAlignment_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + SECONDARY_LINE_2_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // SecondaryLine2, potential field
        fieldMap_mp[keyword_st] = potentialFieldSecondaryLine2Alignment_mat;
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + SECONDARY_LINE_3_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // SecondaryLine3, vector field
        fieldMap_mp[keyword_st] = directionSecondaryLine3VectorFieldAlignment_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + SECONDARY_LINE_3_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // SecondaryLine3, potential field
        fieldMap_mp[keyword_st] = potentialFieldSecondaryLine3Alignment_mat;
        keyword_st = FIELD_TYPE_KEYWORD_VEC_st + TRANSIT_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // Transit, vector field
        fieldMap_mp[keyword_st] = directionTransitVectorFieldAlignment_mat;
        keyword_st = FIELD_TYPE_KEYWORD_POT_st + TRANSIT_IMAGE_NAME_st + FIELD_NAME_POSTFIX_ALIGN_st; // Transit, potential field
        fieldMap_mp[keyword_st] = potentialFieldTransitAlignment_mat;

    } catch (const exception &e) {

        ROS_ERROR("[%s]: Expetion (%s) thrown while mapping the fields.", ros::this_node::getName().c_str(),
                  e.what());

        return false;

    }

    return true;

}

/// searchForImage
/// \param field_mat
/// \param fieldTypeKeyword_st
/// \param fieldNamePostfix_st
/// \param fieldImageName_st
/// \return
bool getFieldByKey_b(cv::Mat& field_mat, string fieldTypeKeyword_st, string fieldNamePostfix_st, string fieldImageName_st) {

    if (fieldMap_mp.size() > 0) {

        string key = fieldTypeKeyword_st + fieldImageName_st + fieldNamePostfix_st;

        if (fieldMap_mp.count(key) > 0) { // check if the key exists in the fieldMap_mp

            field_mat = fieldMap_mp.find(key)->second;

            return true;

        }

    }

    return false;

}

/// ROS SERVICE CALLBACKS

/// imagePreprocessing
/// \param req_c
/// \param res_c
/// \return true if succeed else false
bool imagePreprocessingService_b(aal_potential_field_navigation::initiate_image_routine::Request &req_c,
                        aal_potential_field_navigation::initiate_image_routine::Response &res_c) {

    ROS_INFO("[%s]: imagePreprocessingService_b is called ...", ros::this_node::getName().c_str());

    if (imageRoutineDone_b) { // check if the routine is already done

        ROS_INFO("[%s]: imagePreprocessingService_b is called although the routine is already done, call rejected ...", ros::this_node::getName().c_str());

        res_c.errorCode_i = ErrorCode::NO_ERROR; // no error occured
        res_c.jobDone_i = true; // quit the job successfully

        return true;

    }

    res_c.jobDone_i = false; // as a precaution

    imageRoutineDone_b = false; // as as precaution

    ROS_INFO("[%s]: imagePreprocessingService_b/loadImages_b()...", ros::this_node::getName().c_str());

    if (!loadImages_b()) { // load direction images and check for errors

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::LOAD_IMAGE_ERROR); // concretize the failure
        return false; // response a failure

    }

    ROS_INFO("[%s]: imagePreprocessingService_b/redyeDirectionImages_b()...", ros::this_node::getName().c_str());

    if (!redyeDirectionImages_b()) { // redye (current) the direction images and check for errors

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::REDYE_IMAGE_ERROR); // concretize the failure
        return false; // response a failure

    }

    ROS_INFO("[%s]: imagePreprocessingService_b/redyeAlignmentImages_b()...", ros::this_node::getName().c_str());

    if(!redyeAlignmentImages_b()) { // redye (charge) the direction images and check for errors

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::REDYE_IMAGE_ERROR); // concretize the failure
        return false; // response a failure

    }

    ROS_INFO("[%s]: imagePreprocessingService_b/convertImageToVectorfield_b()...", ros::this_node::getName().c_str());

    if(!convertImageToVectorfield_b()) { // convert all redyed images to vector- and potentialfields

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::CONVERT_IMAGE_TO_FIELD_ERROR); // concretize the failure
        return false; // response a failure

    }

    ROS_INFO("[%s]: imagePreprocessingService_b/mapTheFields_b()...", ros::this_node::getName().c_str());

    if(!mapTheFields_b()) { // store the fields in a capable dictionary

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::MAP_THE_FIELDS_ERROR); // concretize the failure
        return false; // response a failure

    }

    ROS_INFO("[%s]: initial image routine successfully finished ...", ros::this_node::getName().c_str());

    imageRoutineDone_b = true; // set a global flag if the image routine finish correctly

    res_c.errorCode_i = ErrorCode::NO_ERROR; // no error occured
    res_c.jobDone_i = true; // quit the job successfully

    return true;

}

/// callForPublishing
/// \param req_c
/// \param res_c
/// \return true if succeed else false
bool getFieldByKeyService_b(aal_potential_field_navigation::get_field_by_key::Request &req_c,
                        aal_potential_field_navigation::get_field_by_key::Response &res_c) {

    ROS_INFO("[%s]: getFieldByKeyService_b is called ...", ros::this_node::getName().c_str());

    res_c.jobDone_i = false; // as a precaution

    if(!imageRoutineDone_b) { // check if the initial image routine is already done

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::IMAGE_ROUTINE_FLAG_ERROR); // concretize the failure
        return false;

    }

    ROS_INFO("[%s]: getFieldByKeyService_b/getFieldByKey_b() ...", ros::this_node::getName().c_str());

    cv::Mat field_mat;

    ROS_INFO("key = %s", req_c.fieldImageName_st.c_str());

    if (!getFieldByKey_b(field_mat, req_c.fieldTypeKeyword_st, req_c.fieldNamePostfix_st, req_c.fieldImageName_st)) { // get the appropriate field by key

        res_c.errorCode_i = static_cast<int8_t>(ErrorCode::GET_FIELD_BY_KEY_ERROR); // concretize the failure
        return false;

    }

    cv_bridge::CvImage cvImage_c;

    if (req_c.fieldTypeKeyword_st.compare(FIELD_TYPE_KEYWORD_VEC_st) == 0) { // check the field type and define the appropriate encoding

        cvImage_c.encoding = sensor_msgs::image_encodings::TYPE_32FC2;

    } else if (req_c.fieldTypeKeyword_st.compare(FIELD_TYPE_KEYWORD_POT_st) == 0) {

        cvImage_c.encoding = sensor_msgs::image_encodings::TYPE_32FC1;

    }

    field_mat.copyTo(cvImage_c.image);

    res_c.field_c = *cvImage_c.toImageMsg(); // pass field through

    ROS_INFO("[%s]: getFieldByKeyService_b successfully finished ...", ros::this_node::getName().c_str());

    res_c.errorCode_i = ErrorCode::NO_ERROR; // no error occured
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
    ros::NodeHandle node_c("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());
    ros::Rate loopRate_c(LOOP_RATE_i);

    // ROS service advertise
    ros::ServiceServer initiateImageRoutineServiceServer_c = node_c.advertiseService(INITIATE_IMAGE_ROUTINE_st, imagePreprocessingService_b);
    ros::ServiceServer getFieldByKeyServiceServer_c = node_c.advertiseService(GET_FIELD_BY_KEY_st, getFieldByKeyService_b);

    ros::spin(); // keep this node alive

    return 0;

}