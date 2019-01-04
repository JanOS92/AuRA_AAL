/// ==============================================================================
/// \file potential_field_utils.h
/// \brief Provides some useful, generic functions
/// \author Daniel Rudolph <drudolph@techfak.uni-bielefeld.de>
/// \author Timo Korthals <tkorthals@cit-ec.uni-bielefeld.de>
/// \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
/// ==============================================================================

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_POTENTIAL_FIELD_UTILS_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_POTENTIAL_FIELD_UTILS_H

/// INCLUDES

// OpenCV
#include <omp.h>
#include <opencv2/opencv.hpp>

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Pose.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <fstream>

// Types
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/Constants.h"

/// INTERFACE FUNCTIONS

/// write_image_binary
/// \param img Image to store
/// \param name Path
/// \param depth size of variable (char=1, float=4, ...)
/// \brief Stores the image as binary blob
extern void write_image_binary(cv::Mat &img, const std::string &name, int depth = 1 /*4 for float/int, 8 for double, ...*/);

/// getAngleDiff
/// \param origin Angle taken as reference
/// \param target Angle taken as target
/// \return Minimum difference
/// \brief Returns the minimum angle difference between the origin and target
double getAngleDiff(double origin, double target);

/// bgr_to_gray
/// \param bgr Input image
/// \return Output gray image
/// \brief Convert bgr to gray image without scaling
cv::Mat bgr_to_gray(const cv::Mat &image);

/// rgb_to_gray
/// \param rgb Input image
/// \return Output gray image
/// \brief Convert rgb to gray image without scaling
cv::Mat rgb_to_gray(const cv::Mat &image);

/// get_abs_field
/// \param vectorfield CV_32FC2 field
/// \return CV_32FC1 field with corresponding absolute values
/// \brief Normalizes the lenght of all vectors to the largest present vector
cv::Mat get_abs_field(cv::Mat &vectorfield);

/// calc_normalized_field
/// \param vectorfield CV_32FC2 field to normalize
/// \brief Normalizes the lenght of all vectors to the largest present vector
void calc_normalized_field(cv::Mat &vectorfield);

/// vectorfield_to_gray
/// \param vectorfield CV_32FC2 field
/// \return The gray channel
/// \brief Calculates a gray channel out of a vector field
cv::Mat vectorfield_to_gray(const cv::Mat &vectorfield);

/// vectorfield_to_hsv
/// \param vectorField Input vectorfield
/// \return Output hsv image
/// \brief Takes a 2D CV_32FC2 Vectorfield and convertes with the hsv image encoding to a bgr mat
extern cv::Mat vectorfield_to_hsv(const cv::Mat &vectorField, std::string id = "");

/// hsv_to_gray
/// \param hsv Input image
/// \return Output gray image
/// \brief Convert hsv to gray image
inline cv::Mat hsv_to_gray(const cv::Mat &hsv) {

    cv::Mat gray(hsv.rows, hsv.cols, CV_8UC1);

    for (int idx = 0; idx < hsv.rows * hsv.cols; idx++) {

        gray.at<uchar>(idx) = hsv.at<cv::Vec3b>(idx)[2]; // Get the value V

    }

    return gray;
}

/// hsv_to_bgr
/// \param hsv Input image
/// \return Output bgr image
/// \brief Convert hsv to bgr image
inline cv::Mat hsv_to_bgr(const cv::Mat &hsv) {

    cv::Mat bgr(hsv.size(), CV_8UC3);

    cv::cvtColor(hsv, bgr, CV_HSV2BGR);

    return bgr;

}

/// hsv_to_rgb
/// \param hsv Input image
/// \return Output rgb image
/// \brief Convert hsv to rgb image
inline cv::Mat hsv_to_rgb(const cv::Mat &hsv) {

    cv::Mat rgb(hsv.size(), CV_8UC3);

    cv::cvtColor(hsv, rgb, CV_HSV2RGB);

    return rgb;

}

/// potentialfield_to_vectorfield
/// \param potentialField Input potential field
/// \param ddepth Derivative parameter
/// \param anchor Derivative parameter
/// \param delta Derivative parameter
/// \param borderType Derivative parameter
/// \return Output CV_32FC2 vector field
/// \brief Calculates the common derivatives as 2D CV_32FC2 vector field of a potential field
extern cv::Mat potentialfield_to_vectorfield(const cv::Mat &potentialField,
                              const int ddepth = -1 /*CV_32*/,
                              const double delta = 0,
                              int borderType = cv::BORDER_REPLICATE);

/// rot90
/// \param matImage Input image
/// \param rotflag Rotation flag ROTATE_90_CLOCKWISE, ROTATE_90_COUNTERCLOCKWISE or ROTATE_180
/// \brief rotate the "matImage" in dependency of "rotflag"
void rot90(cv::Mat &matImage, int rotflag);

/// pose2pixel
/// \param pose_c
/// \param imageWidth_i
/// \param imageHeight_i
/// \param meterPerPixelWidth_d
/// \param meterPerPixelHeight_d
/// \return point in space
/// \brief Convert the pose from cartesian into pixel space
cv::Point2i pose2pixel(const geometry_msgs::Pose &pose_c, const int imageWidth_i, const int imageHeight_i,
                       const double meterPerPixelWidth_d, const double meterPerPixelHeight_d);

/// rot90Vector
/// \param vector
/// \return
/// \brief Rotate the "vector" by 90 degree ccw
void rot90Vector_v(cv::Vec2f &vector);

/// flipVectors_v
/// \param vectorField_mat
/// \brief Flip the y coordinate of all vectors
void flipVectors_v(cv::Mat &vectorField_mat);

/// flipVector_v
/// \param vector_c
/// \brief Flip the y coordinate of the vector
void flipVector_v(cv::Point2f &vector_c);

/// rotVectors_v
/// \param vectorField_mat
/// \param rotflag Rotation flag ROTATE_90_CLOCKWISE, ROTATE_90_COUNTERCLOCKWISE or ROTATE_180
/// \brief Rotate all vectors in dependency of the rotation flag
void rotVectors_v(cv::Mat &vectorField_mat, int rotFlag);

/// neutralizeVectorsOnGrayPx_v
/// \param vectorField_mat
/// \param bgrImage_mat
/// \brief Set all vectors on gray pixels to 0
void neutralizeVectorsOnGrayPx_v(cv::Mat &vectorField_mat, cv::Mat* bgrImage_mat);

/// drawGradientOnImg_v
/// \param vectorField_mat
/// \param image_mat
/// \brief Draw the gradient vectors of vectorfield on the input image
void drawGradientOnImg_v(cv::Mat vectorField_mat, cv::Mat& image_mat);

/// transformVector_v
/// \param vector_c
/// \brief Transform the vector from the image frame into the world frame
void transformVector_v(cv::Point2f &vector_c);

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_POTENTIAL_FIELD_UTILS_H
