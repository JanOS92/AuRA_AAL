/**
 * ==============================================================================
 * \file GetMotionVector.h
 * \brief Provides the declaration of the perceptual schema class GetMotionVector
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_GETMOTIONVECTOR_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_GETMOTIONVECTOR_H

/// INCLUDES

// Types
#include "aal_potential_field_navigation/perceptual_schemas/PerceptualSchema.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class GetMotionVector : public PerceptualSchema {

public:

    /// GLOBAL METHODS

    /// GetMotionVector
    /// \param topic_st
    /// \brief Constructor
    GetMotionVector(string topic_st);

    /// GetMotionVector
    /// \brief Destructor
    ~GetMotionVector();

    /// getPerceptualData_b
    /// \return motion vector
    /// \brief Return the current motion vector
    MotionVector getPerceptualData_s();

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    void motionVectorCallback_v(const aal_potential_field_navigation::motionVectorMessage::ConstPtr& msg_c); // motion vector callback

    /// LOCAL ATTRIBUTES

    MotionVector motionVec_s;

    ros::Subscriber subMotionVec_c; // motion vector subscriber

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_GETMOTIONVECTOR_H
