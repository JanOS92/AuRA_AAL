/**
 * ==============================================================================
 * \file move_robot_node.cpp
 * \brief Provides an action server for the instantiation of the move-robot
 *        motor schema
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include "ros/ros.h"

// ROS action
#include "../../../../install/include/aal_potential_field_navigation/move_robotAction.h"
#include <actionlib/server/simple_action_server.h>

// Types and classes
#include "aal_potential_field_navigation/motor_schemas/MoveRobot.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

typedef actionlib::SimpleActionServer<aal_potential_field_navigation::move_robotAction> MoveRobotActionServer_c;
aal_potential_field_navigation::move_robotResult MoveRobotResult_c;

/// LOCAL FUNCTIONS

/// reportExeError_v
/// \param actionServer_c
/// \brief Report the execution error to the client
void reportExeError_v(MoveRobotActionServer_c* actionServer_c) {

    MoveRobotResult_c.errorCode_i = static_cast<int8_t>(ErrorCode::BEHAVIOR_EXECUTION_ERROR); // concretize the failure

    actionServer_c->setAborted(MoveRobotResult_c); // set the action server state to "Aborted"

}

/// ROS ACTION

/// moveRobotAction_v
/// \param goal_c
/// \param actionServer_c
void moveRobotAction_v(const aal_potential_field_navigation::move_robotGoalConstPtr& goal_c,
                          MoveRobotActionServer_c* actionServer_c) {

    ros::Rate loopRate_c(LOOP_RATE_i);

    MoveRobotResult_c.errorCode_i = NO_ERROR; // set default

    ROS_INFO("[%s]: moveRobotAction_v is called...", ros::this_node::getName().c_str());

    MoveRobot moveRobot_c(goal_c->id_ui);

    ROS_INFO("[%s]: instantiation of the MoveRobot behavior finished successfully...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: calling the MoveRobot behavior...", ros::this_node::getName().c_str());

    while (ros::ok()) { // execution loop

        ros::spinOnce();

        if (!moveRobot_c.executeBehavior_b()) { // execute the behavior

            reportExeError_v(actionServer_c); // call the execution error routine

            return;

        }

        if (actionServer_c->isPreemptRequested()) { // check if the goal are canceled by the client

            ROS_INFO("[%s]: stopping robot...", ros::this_node::getName().c_str());

            moveRobot_c.stopRobot_v();

            ROS_INFO("[%s]: deinstantiate move-robot motor schema and close the action...", ros::this_node::getName().c_str());

//            actionServer_c->setPreempted(MoveRobotResult_c); // set the action server state to "Preempted"

            break;

        }

        // ToDo: Cause this any problems?
//        loopRate_c.sleep();

    }

    ROS_INFO("[%s]: MoveRobot behavior successfully finished...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: call moveRobotAction_v successfully finished...", ros::this_node::getName().c_str());

    MoveRobotResult_c.errorCode_i = NO_ERROR; // quit the job without causing an error handling

    actionServer_c->setSucceeded(MoveRobotResult_c); // set the server state to "Succeeded"

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

    // ROS action (server)
    MoveRobotActionServer_c actionServer_c(node_c, MOVE_ROBOT_st, boost::bind(&moveRobotAction_v, _1, &actionServer_c), false);
    actionServer_c.start();

    ROS_INFO("[%s] MoveRobot action server is online", ros::this_node::getName().c_str());

    ros::spin(); // keep this node alive

    return 0;

}