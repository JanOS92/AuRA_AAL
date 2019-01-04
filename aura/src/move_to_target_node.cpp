/**
 * ==============================================================================
 * \file move_to_target_node.cpp
 * \brief Provides an action server for the instantiation of the move-to-target
 *        motor schema
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include "ros/ros.h"

// ROS service
#include "../../../../install/include/aal_potential_field_navigation/call_move_to_target.h"
#include "../../../../install/include/aal_potential_field_navigation/get_field_by_key.h"

// ROS action
#include "../../../../install/include/aal_potential_field_navigation/move_to_targetAction.h"
#include <actionlib/server/simple_action_server.h>

// Types and classes
#include "aal_potential_field_navigation/motor_schemas/MoveToTarget.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

ros::ServiceClient getFieldByKeyServiceClient_c;

tf::TransformListener *listener_c;

typedef actionlib::SimpleActionServer<aal_potential_field_navigation::move_to_targetAction> MoveToTargetActionServer_c;
aal_potential_field_navigation::move_to_targetFeedback MoveToTargetFeedback_c;
aal_potential_field_navigation::move_to_targetResult MoveToTargetResult_c;
boost::shared_ptr<const typename actionlib::SimpleActionServer<aal_potential_field_navigation::move_to_targetAction>::Goal> newGoalPtr_pc;

/// LOCAL FUNCTIONS

/// reportInitError_v
/// \param actionServer_c
/// \brief Report the initialization error to the client
void reportInitError_v(MoveToTargetActionServer_c* actionServer_c) {

//    MoveToTargetFeedback_c.jobDone_i = false; // quit the job with an error
    MoveToTargetResult_c.errorCode_i = static_cast<int8_t>(ErrorCode::BEHAVIOR_INITIALIZATION_ERROR); // concretize the failure

//    actionServer_c->publishFeedback(MoveToTargetFeedback_c); // publish the feedback
    actionServer_c->setAborted(MoveToTargetResult_c); // set the action server state to "Aborted"

}

/// reportExeError_v
/// \param actionServer_c
/// \brief Report the execution error to the client
void reportExeError_v(MoveToTargetActionServer_c* actionServer_c) {

//    MoveToTargetFeedback_c.jobDone_i = false; // quit the job with an error
    MoveToTargetResult_c.errorCode_i = static_cast<int8_t>(ErrorCode::BEHAVIOR_EXECUTION_ERROR); // concretize the failure

//    actionServer_c->publishFeedback(MoveToTargetFeedback_c); // publish the feedback
    actionServer_c->setAborted(MoveToTargetResult_c); // set the action server state to "Aborted"

}

/// ROS ACTION

/// moveToTargetAction_v
/// \param goal_c
/// \param actionServer_c
void moveToTargetAction_v(const aal_potential_field_navigation::move_to_targetGoalConstPtr& goal_c,
                              MoveToTargetActionServer_c* actionServer_c) {

    ros::Rate loopRate_c(LOOP_RATE_i);

    MoveToTargetResult_c.errorCode_i = NO_ERROR; // set default

    ROS_INFO("[%s]: moveToTargetAction_v is called ...", ros::this_node::getName().c_str());

    MoveToTarget moveToTarget_c(goal_c->id_ui, goal_c->finalTargetRFID_ui, (JobLogic)goal_c->checkJobLogic_i, actionServer_c, listener_c); // instantiate a MoveToTarget object

    ROS_INFO("[%s]: instantiation of the MoveToTarget behavior finished successfully...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: localizing...", ros::this_node::getName().c_str());

    while (!moveToTarget_c.isLocalized_b()) { // wait until the schema localizes a RFID

        ros::spinOnce();

        loopRate_c.sleep();

    }

    ROS_INFO("[%s]: calling the MoveToTarget behavior...", ros::this_node::getName().c_str());

    if(!moveToTarget_c.init_b()) { // initialize the behavior

        reportInitError_v(actionServer_c); // call the initialization error routine

        return;

    }

    while (ros::ok()) { // execution loop

        ros::spinOnce();

        moveToTarget_c.checkJob_v(); // check/report the job status

        if (!moveToTarget_c.executeBehavior_b()) { // execute the behavior

            reportExeError_v(actionServer_c); // call the execution error routine

            return;

        }

        if (actionServer_c->isNewGoalAvailable()) { // check if a new goal is set by the client

            ROS_INFO("[%s]: move-to-target motor schema accept the new goal...", ros::this_node::getName().c_str());

            newGoalPtr_pc = actionServer_c->acceptNewGoal(); // get the new goal

            moveToTarget_c.setJobLogic_v((JobLogic)newGoalPtr_pc->checkJobLogic_i); // set the new expected RFID action
            moveToTarget_c.setTargetRFID_v(newGoalPtr_pc->finalTargetRFID_ui); // set the new final target RFID

            if (!moveToTarget_c.init_b()) { // reinitialize the motor schema in dependency of the new target

                reportInitError_v(actionServer_c); // call the initialization error routine

                return;

            }

            newGoalPtr_pc.reset(); // reset the new goal pointer

        }

        if (actionServer_c->isPreemptRequested()) { // check if the goal are canceled by the client

            ROS_INFO("[%s]: deinstantiate move-to-target motor schema and close the action...", ros::this_node::getName().c_str());

//            actionServer_c->setPreempted(MoveToTargetResult_c); // set the action server state to "Preempted"

            break;

        }

        loopRate_c.sleep();

    }

    ROS_INFO("[%s]: MoveToTarget behavior successfully finished...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: call MoveToTargetService_b successfully finished ...", ros::this_node::getName().c_str());

    MoveToTargetResult_c.errorCode_i = NO_ERROR; // quit the job without causing an error handling

    actionServer_c->setSucceeded(MoveToTargetResult_c); // set the action server state to "Succeeded"

}

/// MAIN

/// main
/// \param argc_i
/// \param argv_pch
/// \return 0
int main(int argc_i, char *argv_pch[]) {

    // init ROS
    ros::init(argc_i, argv_pch, ros::this_node::getName());
    ros::NodeHandle node_c("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());
    ros::Rate loopRate_c(LOOP_RATE_i);

    // ROS tf
    listener_c = new tf::TransformListener();

    // ROS action (server)
    MoveToTargetActionServer_c actionServer_c(node_c, MOVE_TO_TARGET_st, boost::bind(&moveToTargetAction_v, _1, &actionServer_c), false);
    actionServer_c.start();

    ROS_INFO("[%s] MoveToTarget action server is online", ros::this_node::getName().c_str());

    // ROS service subscribe (client)
    getFieldByKeyServiceClient_c = node_c.serviceClient<aal_potential_field_navigation::get_field_by_key>(GET_FIELD_BY_KEY_st);

    ros::spin(); // keep this node alive

    delete(listener_c);

    return 0;

}