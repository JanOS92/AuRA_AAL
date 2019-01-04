/**
 * ==============================================================================
 * \file stay_on_path_node.cpp
 * \brief Provides an action server for the instantiation of the stay-on-path
 *        motor schema
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include "ros/ros.h"

// ROS action
#include "../../../../install/include/aal_potential_field_navigation/stay_on_pathAction.h"
#include <actionlib/server/simple_action_server.h>

// Types and classes
#include "aal_potential_field_navigation/motor_schemas/StayOnPath.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

tf::TransformListener *listener_c;

typedef actionlib::SimpleActionServer<aal_potential_field_navigation::stay_on_pathAction> StayOnPathActionServer_c;
aal_potential_field_navigation::stay_on_pathResult StayOnPathResult_c;
boost::shared_ptr<const typename actionlib::SimpleActionServer<aal_potential_field_navigation::stay_on_pathAction>::Goal> newGoalPtr_pc;

/// LOCAL FUNCTIONS

/// reportInitError_v
/// \param actionServer_c
/// \brief Report the initialization error to the client
void reportInitError_v(StayOnPathActionServer_c* actionServer_c) {

    StayOnPathResult_c.errorCode_i = static_cast<int8_t>(ErrorCode::BEHAVIOR_INITIALIZATION_ERROR); // concretize the failure

    actionServer_c->setAborted(StayOnPathResult_c); // set the action server state to "Aborted"

}

/// reportExeError_v
/// \param actionServer_c
/// \brief Report the execution error to the client
void reportExeError_v(StayOnPathActionServer_c* actionServer_c) {

    StayOnPathResult_c.errorCode_i = static_cast<int8_t>(ErrorCode::BEHAVIOR_EXECUTION_ERROR); // concretize the failure

    actionServer_c->setAborted(StayOnPathResult_c); // set the action server state to "Aborted"

}

/// ROS ACTION

/// stayOnPathAction_v
/// \param goal_c
/// \param actionServer_c
void stayOnPathAction_v(const aal_potential_field_navigation::stay_on_pathGoalConstPtr& goal_c,
                        StayOnPathActionServer_c* actionServer_c) {

    ros::Rate loopRate_c(LOOP_RATE_i);

    StayOnPathResult_c.errorCode_i = NO_ERROR; // set default

    ROS_INFO("[%s]: stayOnPathAction_v is called ...", ros::this_node::getName().c_str());

    StayOnPath stayOnPath_c(goal_c->id_ui, listener_c); // instantiate a StayOnPath object

    ROS_INFO("[%s]: instantiation of the StayOnPath behavior finished successfully...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: localizing...", ros::this_node::getName().c_str());

    while (!stayOnPath_c.isLocalized_b()) { // wait until the schema localizes a RFID

        ros::spinOnce();

        loopRate_c.sleep();

    }

    ROS_INFO("[%s]: calling the StayOnPath behavior...", ros::this_node::getName().c_str());

    if(!stayOnPath_c.init_b()) { // initialize the behavior

        reportInitError_v(actionServer_c); // call the initialization error routine

        return;

    }

    while (ros::ok()) { // execution loop

        ros::spinOnce();

        if (!stayOnPath_c.executeBehavior_b()) { // execute the behavior

            reportExeError_v(actionServer_c); // call the execution error routine

            return;

        }

        if (actionServer_c->isNewGoalAvailable()) { // check if a new goal is set by the client

            ROS_INFO("[%s]: stay-on-path motor schema accept the new goal...", ros::this_node::getName().c_str());

            newGoalPtr_pc = actionServer_c->acceptNewGoal(); // get the new goal

            stayOnPath_c.setTargetRFID_v(newGoalPtr_pc->finalTargetRFID_ui); // set the new final target RFID

            if (!stayOnPath_c.init_b()) { // reinitialize the motor schema in dependency of the new target

                reportInitError_v(actionServer_c); // call the initialization error routine

                return;

            }

            newGoalPtr_pc.reset(); // reset the new goal pointer

        }

        if (actionServer_c->isPreemptRequested()) { // check if the goal are canceled by the client

            ROS_INFO("[%s]: deinstantiate stay-on-path motor schema and close the action...", ros::this_node::getName().c_str());

//            actionServer_c->setPreempted(StayOnPathResult_c); // set the action server state to "Preempted"

            break;

        }

        loopRate_c.sleep();

    }

    ROS_INFO("[%s]: StayOnPath behavior successfully finished...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: call stayOnPathAction_v successfully finished ...", ros::this_node::getName().c_str());

    StayOnPathResult_c.errorCode_i = NO_ERROR; // quit the job without causing an error handling

    actionServer_c->setSucceeded(StayOnPathResult_c); // set the server state to "Succeeded"

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
    StayOnPathActionServer_c actionServer_c(node_c, STAY_ON_PATH_st, boost::bind(&stayOnPathAction_v, _1, &actionServer_c), false);
    actionServer_c.start();

    ROS_INFO("[%s] StayOnPath action server is online", ros::this_node::getName().c_str());

    ros::spin(); // keep this node alive

    delete(listener_c);

    return 0;

}