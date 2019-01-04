/**
 * ==============================================================================
 * \file StayOnPath.h
 * \brief Provides the declaration of the motor schema class StayOnPath
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_STAYONPATH_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_STAYONPATH_H

/// INCLUDES

// Types
#include "MotorSchema.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetRFID.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetAlignment.h"

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class StayOnPath : public MotorSchema {

public:

    /// GLOBAL METHODS

    /// StayOnPath
    /// \param id_ui
    /// \brief Constructor
    StayOnPath(uint64_t id_ui, tf::TransformListener *listener_c);

    /// ~StayOnPath
    /// \brief Destructor
    ~StayOnPath();

    /// isLocalized_b
    /// \return true if an initial RFID and a position (x,y) are already registered
    /// \brief Check for localization
    bool isLocalized_b();

    /// init_b
    /// \return true if succeed else false
    /// \brief Initialize the specific motor schema behavior
    bool init_b();

    /// executeBehavior_b
    /// \return true if succeed else false
    /// \brief Determine the kinematic command and transmit them to the move-robot schema
    bool executeBehavior_b();

    /// visualizeVectorfield_v
    /// \param -
    /// \return -
    /// \brief Publish the vectorfield to the visualization node (vectorfield_to_gridmap)
    void visualizeVectorfield_v();

    /// setTargetRFID_v
    /// \param targetRFID_ui
    /// \brief Set the member attribute targetRFID_ui
    void setTargetRFID_v(uint64_t targetRFID_ui);

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// mapPoseToVectorfield_c
    /// \return point in space
    /// \brief Map the pose respectively the position to the vectorfield
    cv::Point2i mapPoseToVectorfield_c();

    /// LOCAL ATTRIBUTES

    uint64_t targetRFID_ui;

    uint64_t initialRFID_ui;
    cv::Mat vectorfield_mat;
    image_transport::Publisher vectorfieldPub_c;
    image_transport::ImageTransport imageTransport_c;
    tf::TransformListener *listener_c;

    /// PERCEPTUAL SCHEMAS

    GetRFID getRFID_c;
    GetPosition getPosition_c;
    GetAlignment getAlignment_c;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_STAYONPATH_H
