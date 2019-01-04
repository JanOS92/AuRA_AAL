/// ==============================================================================
/// @file navigation_utils.cpp
/// @brief Provides some useful functions for navigation
/// @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
/// ==============================================================================

/// INCLUDES

#include "aal_potential_field_navigation/navigation/navigation_utils.h"

/// NAMESPACES

/// LOCAL FUNCTIONS

/// rfidActionToFieldKey_st
/// -----------------------------------------------------------------
/// additional information are given in the navigation_utils.h
/// -----------------------------------------------------------------
std::string rfidActionToFieldKey_st(RFIDAction rfidAction, uint64_t targetRFID_ui, uint64_t initialRFID_ui) {

    if (rfidAction == RFIDAction::START) { // case start

        if (initialRFID_ui == RFID_STOCK2_ui) { // start at Stock 2

            if (CCW_i) { // ccw direction

                return SECONDARY_LINE_1_IMAGE_NAME_st;

            } else { // cw direction

                return SECONDARY_LINE_2_IMAGE_NAME_st;

            }

        } else if (initialRFID_ui == RFID_STOCK1_ui) { // start at Stock 1

            if (CCW_i) { // ccw direction

                return SECONDARY_LINE_3_IMAGE_NAME_st;

            } else { // cw direction

                return SECONDARY_LINE_1_IMAGE_NAME_st;

            }

        }  else { // otherwise start at Testing, Assembly 1 or Assembly 2

            return SECONDARY_LINE_1_IMAGE_NAME_st; // in either direction

        }

    } else if (rfidAction == RFIDAction::ASCEND) { // case ascend on transit

        if (CCW_i && initialRFID_ui == RFID_STOCK2_ui) { // leave Stock 2 in direction ccw

            return  SECONDARY_LINE_1_IMAGE_NAME_st;

        } else if (!CCW_i && initialRFID_ui == RFID_STOCK1_ui) { // leave Stock 1 in direction cw

            return SECONDARY_LINE_1_IMAGE_NAME_st;

        } else if (!CCW_i && initialRFID_ui == RFID_STOCK2_ui) { // leave Stock 2 in direction cw

            return SECONDARY_LINE_2_IMAGE_NAME_st;

        } else if (CCW_i && initialRFID_ui == RFID_STOCK1_ui) { // leave Stock 1 in direction ccw

            return SECONDARY_LINE_3_IMAGE_NAME_st;

        } else { // leave Testing, Assembly 1 or Assembly 2

            return SECONDARY_LINE_1_IMAGE_NAME_st;

        }

    } else if (rfidAction == RFIDAction::DESCEND) { // case descend

        if (targetRFID_ui == RFID_STOCK1_ui) { // target is Stock 1

            if (CCW_i) { // ccw direction

                return SECONDARY_LINE_1_IMAGE_NAME_st;

            } else { // cw direction

                return SECONDARY_LINE_3_IMAGE_NAME_st;

            }

        } else if (targetRFID_ui == RFID_STOCK2_ui) { // target is Stock 2

            if (CCW_i) { // ccw direction

                return SECONDARY_LINE_2_IMAGE_NAME_st;

            } else { // cw direction

                return SECONDARY_LINE_1_IMAGE_NAME_st;

            }

        } else if (targetRFID_ui == RFID_ASSEMBLY1_ui || targetRFID_ui == RFID_ASSEMBLY2_ui ||
                   targetRFID_ui == RFID_TESTING_ui) { // target is Testing, Assembly 1 or Assembly 2

            return SECONDARY_LINE_1_IMAGE_NAME_st; // in either direction

        } else { // transit otherwise

            return TRANSIT_IMAGE_NAME_st;

        }

    } else if (rfidAction == RFIDAction::STOP) { // case stop

        return DEFAULT_FIELD_KEY_st;

    } else if (rfidAction == RFIDAction::TRANSIT) { // case transit

        return TRANSIT_IMAGE_NAME_st;

    } else if (rfidAction == RFIDAction::NOTHING) { // default case

        return DEFAULT_FIELD_KEY_st;

    }

    return DEFAULT_FIELD_KEY_st; // to avoid warnings

}