/**
 * ==============================================================================
 * \file WaitForObstacle.h
 * \brief Provides the declaration of the motor schema class WaitForObstacle
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_WAITFOROBSTACLE_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_WAITFOROBSTACLE_H

/// INCLUDES

// Types
#include "MotorSchema.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetRingValues.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class WaitForObstacle : public MotorSchema {

public:

    /// GLOBAL METHODS

    /// WaitForObstacle
    /// \param id_ui
    /// \brief Constructor
    WaitForObstacle(uint64_t id_ui);

    /// WaitForObstacle
    /// \brief Destructor
    ~WaitForObstacle();

    /// executeBehavior_b
    /// \return true if succeed else false
    /// \brief Determine the kinematic command and transmit them to the move-robot schema
    bool executeBehavior_b();

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// LOCAL ATTRIBUTES

    /// PERCEPTUAL SCHEMAS

    GetRingValues getRingValues_c;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_WAITFOROBSTACLE_H
