/**
 * ==============================================================================
 * \file MoveToTarget.h
 * \brief Provides the declaration of the motor schema class MoveToTarget
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

#ifndef AAL_POTENTIAL_FIELD_NAVIGATION_MOVETOTARGET_H
#define AAL_POTENTIAL_FIELD_NAVIGATION_MOVETOTARGET_H

/// INCLUDES

// Types
#include "MotorSchema.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetDirection.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetRFID.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"
#include "aal_potential_field_navigation/perceptual_schemas/GetFloorValues.h"

// ROS action
#include "../../../../install/include/aal_potential_field_navigation/move_to_targetAction.h"
#include <actionlib/server/simple_action_server.h>

// Utils
#include <functional>

/// Types

typedef actionlib::SimpleActionServer<aal_potential_field_navigation::move_to_targetAction> moveToTargetActionServer_c;

/// NAMESPACES

using namespace std;

/// CLASS DECLARATION

class MoveToTarget : public MotorSchema {

public:

    /// GLOBAL METHODS

    /// MoveToTarget
    /// \param id_ui
    /// \param targetRFID_ui
    /// \param jobLogic_e
    /// \param actionServer_pc
    /// \param listener_c
    /// \brief Constructor
    MoveToTarget(uint64_t id_ui, uint64_t targetRFID_ui, JobLogic jobLogic_e,
                 moveToTargetActionServer_c *actionServer_pc, tf::TransformListener *listener_c);

    /// MoveToTarget
    /// \brief Destructor
    ~MoveToTarget();

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

    /// checkJob_b
    /// \brief Return the current target respectively its RFID action to the client
    void checkJob_v();

    /// setJobLogic_v
    /// \param jobLogic_e
    /// \brief Set the member attribute jobLogic_e
    void setJobLogic_v(JobLogic jobLogic_e);

    /// setTargetRFID_v
    /// \param targetRFID_ui
    /// \brief Set the member attribute targetRFID_ui
    void setTargetRFID_v(uint64_t targetRFID_ui);

    /// GLOBAL ATTRIBUTES

protected:

    /// LOCAL/INHERTIABLE METHODS

    /// visualizeVectorfield_v
    /// \brief Publish the vectorfield to the visualization node (vectorfield_to_gridmap)
    void visualizeVectorfield_v();

    /// LOCAL/INHERTIABLE ATTRIBUTES

private:

    /// LOCAL METHODS

    /// mapPoseToVectorfield_c
    /// \return point in space
    /// \brief Map the pose respectively the position to the vectorfield
    cv::Point2i mapPoseToVectorfield_c();

    /// checkJobDefault_b
    /// \return true if the job is fulfilled else false
    /// \brief Default implementation of the job target
    void checkJobDefault_v();

    /// checkJobGoal_b
    /// \return true if the job is fulfilled else false
    /// \brief Specific implementation (goal) of the job target
    void checkJobGoal_v();

    /// setCheckJobLogic_v
    /// \brief Set the specific job logic
    void setCheckJobLogic_v();

    /// RFIDChecked_b
    /// \param RFID_ui
    /// \return true if the RFID is already checked else false
    /// \brief Check if the input RFID is already checked
    bool RFIDChecked_b(uint64_t RFID_ui);

    /// notifyGoalMissed_v
    /// \brief Notify the meta state machine that the goal is missed
    void notifyGoalMissed_v();

    /// notifyGoalReached_v
    /// \brief Notify the meta state machine that the goal is reached
    void notifyGoalReached_v();

    /// notifyTargetReached_v
    /// \param RFIDAction_e
    /// \brief Notify the meta state machine that the target is reached
    void notifyTargetReached_v(RFIDAction RFIDAction_e);

    /// LOCAL ATTRIBUTES

    uint64_t initialRFID_ui;
    cv::Mat vectorfield_mat;
    image_transport::Publisher vectorfieldPub_c;
    image_transport::ImageTransport imageTransport_c;
    tf::TransformListener *listener_c;
    function<void()> checkJobLogic_v;
    uint64_t targetRFID_ui;
    JobLogic jobLogic_e;
    uint64_t checkedRFID_ui;
    moveToTargetActionServer_c *actionServer_pc;

    /// PERCEPTUAL SCHEMAS

    GetPosition getPosition_c;
    GetFloorValues getFloorValues_c;
    GetDirection getDirection_c;
    GetRFID getRFID_c;

};

#endif //AAL_POTENTIAL_FIELD_NAVIGATION_MOVETOTARGET_H
