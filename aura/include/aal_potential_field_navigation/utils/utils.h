/**
 * ==============================================================================
 * \file Utils.h
 * \brief Provides the declaration of package wide utility functions and includes
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_UTILS_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_UTILS_H

/// INCLUDES

#include <stdint.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <iterator>
#include <map>
#include <list>
#include "boost/tuple/tuple.hpp"
#include "ros/ros.h"
#include <limits>

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

/// LOCAL FUNCTIONS

/// getMinimum_ui
/// \param values_pui
/// \return minimum value
/// \brief Determine the minimum value of a given array
template <class T>
T getMinimum_T(T* values_pui) {

    int size_i = sizeof(values_pui) / sizeof(T);
    T minimum_u = numeric_limits<T>::max();

    for (int idx_i = 0; idx_i <= size_i; idx_i++) {

        if (values_pui[idx_i] < minimum_u) {

            minimum_u = values_pui[idx_i];

        }

    }

    return minimum_u;

}

/// getMaximum_ui
/// \param values_pui
/// \return maximum value
/// \brief Determine the maximum value of a given array
template <class T>
T getMaximum_T(T* values_pui) {

    int size_i = sizeof(values_pui) / sizeof(T);
    T maximum_u = 0;

    for (int idx_i = 0; idx_i <= size_i; idx_i++) {

        if (values_pui[idx_i] > maximum_u) {

            maximum_u = values_pui[idx_i];

        }

    }

    return maximum_u;

}

/// sgn_i
/// \param val
/// \return -1, +1 or 0
/// \brief Determine the sign of the input value
template <class T>
int sgn_i(T val) {

    if(val < 0.0) {

        return -1;

    } else if (val > 0.0) {

        return 1;

    } else {

        return 0;

    }

}


#endif //AAL_POTENTIAL_FIELD_NAVIGATION_UTILS_H
