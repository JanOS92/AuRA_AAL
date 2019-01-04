/// ==============================================================================
/// @file navigation_utils.h
/// @brief Provides some useful functions for navigation
/// @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
/// @date 07.05.2018
/// ==============================================================================

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_NAVIGATION_UTILS_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_NAVIGATION_UTILS_H

/// INCLUDES

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// std library
#include <tuple>
#include <string>

// Utils
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/Types.h"

/// NAMESPACES

/// LOCAL FUNCTIONS

/// rfidActionToFieldKey_st
/// \param rfidAction
/// \param targetRFID_ui
/// \param initialRFID_ui
/// \return appropriate fieldKey
std::string rfidActionToFieldKey_st(RFIDAction rfidAction, uint64_t targetRFID_ui, uint64_t initialRFID_ui);

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_NAVIGATION_UTILS_H
