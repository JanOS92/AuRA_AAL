/**
 * ==============================================================================
 * \file avoid_obstacle_node.cpp
 * \brief Provides an action server for the instantiation of the avoid-obstacle
 *        motor schema
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include "ros/ros.h"

// ROS action
#include "../../../../install/include/aal_potential_field_navigation/avoid_obstacleAction.h"
#include <actionlib/server/simple_action_server.h>

// Types and classes
#include "aal_potential_field_navigation/motor_schemas/AvoidObstacle.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

typedef actionlib::SimpleActionServer<aal_potential_field_navigation::avoid_obstacleAction> AvoidObstacleActionServer_c;
aal_potential_field_navigation::avoid_obstacleResult AvoidObstacleResult_c;
boost::shared_ptr<const typename actionlib::SimpleActionServer<aal_potential_field_navigation::avoid_obstacleAction>::Goal> newGoalPtr_pc;

/// LOCAL FUNCTIONS

/// reportInitError_v
/// \param actionServer_c
/// \brief Report the initialization error to the client
void reportInitError_v(AvoidObstacleActionServer_c* actionServer_c) {

    AvoidObstacleResult_c.errorCode_i = static_cast<int8_t>(ErrorCode::BEHAVIOR_INITIALIZATION_ERROR); // concretize the failure

    actionServer_c->setAborted(AvoidObstacleResult_c); // set the action server state to "Aborted"

}

/// reportExeError_v
/// \param actionServer_c
/// \brief Report the execution error to the client
void reportExeError_v(AvoidObstacleActionServer_c* actionServer_c) {

    AvoidObstacleResult_c.errorCode_i = static_cast<int8_t>(ErrorCode::BEHAVIOR_EXECUTION_ERROR); // concretize the failure

    actionServer_c->setAborted(AvoidObstacleResult_c); // set the action server state to "Aborted"

}

/// ROS ACTION

/// avoidObstacleAction_v
/// \param goal_c
/// \param actionServer_c
void avoidObstacleAction_v(const aal_potential_field_navigation::avoid_obstacleGoalConstPtr &goal_c,
                           AvoidObstacleActionServer_c *actionServer_c) {

    ros::Rate loopRate_c(LOOP_RATE_i);

    AvoidObstacleResult_c.errorCode_i = NO_ERROR; // set default

    ROS_INFO("[%s]: avoidObstacleAction_v is called ...", ros::this_node::getName().c_str());

    AvoidObstacle avoidObstacle_c(goal_c->id_ui); // instantiate a AvoidObstacle object

    ROS_INFO("[%s]: instantiation of the AvoidObstacle behavior finished successfully...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: calling the AvoidObstacle behavior...", ros::this_node::getName().c_str());

    while (ros::ok()) { // execution loop

        ros::spinOnce();

        if (!avoidObstacle_c.executeBehavior_b()) { // execute the behavior

            reportExeError_v(actionServer_c); // call the initialization error routine

            return;

        }

        if (actionServer_c->isPreemptRequested()) { // check if the goal are canceled by the client

            ROS_INFO("[%s]: deinstantiate avoid-obstacle motor schema and close the action...", ros::this_node::getName().c_str());

//            actionServer_c->setPreempted(AvoidObstacleResult_c); // set the action server state to "Preempted"

            break;

        }

        loopRate_c.sleep();

    }

    ROS_INFO("[%s]: AvoidObstacle behavior successfully finished...", ros::this_node::getName().c_str());

    ROS_INFO("[%s]: call avoidObstacleAction_v successfully finished ...", ros::this_node::getName().c_str());

    AvoidObstacleResult_c.errorCode_i = NO_ERROR; // quit the job without causing an error handling

    actionServer_c->setSucceeded(AvoidObstacleResult_c); // set the server state to "Succeeded"

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
    AvoidObstacleActionServer_c actionServer_c(node_c, AVOID_OBSTACLE_st, boost::bind(&avoidObstacleAction_v, _1, &actionServer_c), false);
    actionServer_c.start();

    ROS_INFO("[%s] AvoidObstacle action server is online", ros::this_node::getName().c_str());

    ros::spin(); // keep this node alive

    return 0;

}