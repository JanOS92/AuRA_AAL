/**
 * ==============================================================================
 * \file AvoidObstacle.h
 * \brief Provides the declaration of the motor schema class AvoidObstacle
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_AVOIDOBSTACLE_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_AVOIDOBSTACLE_H

/// INCLUDES

// Types
#include "MotorSchema.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetRingValues.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class AvoidObstacle : public MotorSchema {

public:

    /// GLOBAL METHODS

    /// WaitForObstacle
    /// \param id_ui
    /// \brief Constructor
    AvoidObstacle(uint64_t id_ui);

    /// WaitForObstacle
    /// \brief Destructor
    ~AvoidObstacle();

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

    /// calcFObst_c
    /// \return resulting obstacle avoidance vector
    /// \brief Return the kinematic vector in dependency of the ring sensor values
    cv::Vec2f calcFObst_c();

    /// LOCAL ATTRIBUTES

    /// PERCEPTUAL SCHEMAS

    GetRingValues getRingValues_c;
    GetPosition getPosition_c;

};


#endif //AAL_POTENTIAL_FIELD_NAVIGATION_AVOIDOBSTACLE_H
