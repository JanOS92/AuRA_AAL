/// ==============================================================================
/// @file image_converting_utils.h
/// @brief Provides some useful functions for the image converting node
/// @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
/// @date 11.04.2018
/// ==============================================================================

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_SCHEMA_BACKEND_UTILS_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_SCHEMA_BACKEND_UTILS_H

/// INCLUDES

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// std library
#include <tuple>

// Utils
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/utils/potential_field_utils.hpp"

// Boost
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

/// INTERFACE FUNCTIONS

/// imageToVectorfield_pmat
/// \param rawImage
/// \return dyed image
cv::Mat redyeDirection_mat(cv::Mat rawImage_mat);

/// imageToVectorfield_pmat
/// \param rawImage
/// \return dyed image
cv::Mat redyeAlignment_mat(cv::Mat rawImage_mat);

/// imageToVectorfield_pmat2
/// \param dyedImage
/// \return [0]: vectorfield, [1]: potentialfield
std::pair<cv::Mat,cv::Mat> imageToVectorfield_pmat(cv::Mat dyedImage_mat, std::string imageType_st, bool applyHeuristics_b);

/// redyeDirectionExtended_mat
/// \param rawImage
/// \return dyed image
cv::Mat redyeDirectionExtended_mat(cv::Mat rawImage_mat);

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_SCHEMA_BACKEND_UTILS_H
