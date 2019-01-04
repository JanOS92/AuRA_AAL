#!/usr/bin/env python

# ==============================================================================
# @file enums.py
# @brief Provides a meta interface for the action communication
# @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
# ==============================================================================

## Imports

# include directories
import sys
sys.path.append('../types')

# Types
import parameters as p
import ActionClient as ac

# ROS
import rospy

# ROS: Action
import actionlib
from aal_potential_field_navigation.msg import stay_on_pathAction, stay_on_pathGoal
from aal_potential_field_navigation.msg import move_to_targetAction, move_to_targetGoal, move_to_targetFeedback
from aal_potential_field_navigation.msg import wait_for_obstacleAction, wait_for_obstacleGoal
from aal_potential_field_navigation.msg import avoid_obstacleAction, avoid_obstacleGoal
from aal_potential_field_navigation.msg import move_robotAction, move_robotGoal
from aal_potential_field_navigation.msg import traceAction, traceGoal

## Interface functions

## Description:
# Cause a cancellation of a specific action process
## Parameter:
# actionClient : specific ActionClient object
## Return
# -
def deinstantiateAction(actionClient):

    print("cancelling all goals off: " + actionClient.getName() + "...")

    actionClient.getClientHandler().cancel_all_goals()

## Description:
# Cause a cancellation of all current action processes
## Parameter:
# actionClientDic : dictionary with all ActionClient objects
## Return
# -
def deinstantiateAllActions(actionClientDic):

    for actionClient in actionClientDic.itervalues():

        try:

            deinstantiateAction(actionClient)

        except:

            pass

## Description:
# Determine the action client handler
## Parameter:
# clientName : name of the action client
# clientTopic : name of the action client topic
# actionHandler : action handler object
## Return
# client : client handler object
def getClientHandler(clientName, clientTopic, actionHandler):

    print "initialize action client " + clientName + "..."

    client = actionlib.SimpleActionClient(clientTopic, actionHandler)

    # wait for server response
    if not client.wait_for_server(p.clientTimeOutDuration):

        print("> Timeout while initializing the " + clientName + " client...")
        print(">> clientTopic: " + clientTopic)

        return None

    return client

## Description:
# Initialize the connection between action clients and servers
## Parameter:
# -
## Return
# actionClientDic : dictionary (key : action name) of ActionClient objects
def initializeActionInterface():

    try:

        # move-robot
        actionClientHandler = getClientHandler(p.moveRobotActionName, p.moveRobotActionTopic, move_robotAction)
        moveRobotActionClient = ac.ActionClient(p.moveRobotActionName, actionClientHandler, p.moveRobotActionTopic, move_robotAction, move_robotGoal)

        # move-to-target
        actionClientHandler = getClientHandler(p.moveToTargetActionName, p.moveToTargetActionTopic, move_to_targetAction)
        moveToTargetActionClient = ac.ActionClient(p.moveToTargetActionName, actionClientHandler, p.moveToTargetActionTopic, move_to_targetAction, move_to_targetGoal)

        # stay-on-path
        actionClientHandler = getClientHandler(p.stayOnPathActionName, p.stayOnPathActionTopic, stay_on_pathAction)
        stayOnPathActionClient = ac.ActionClient(p.stayOnPathActionName, actionClientHandler, p.stayOnPathActionTopic, stay_on_pathAction, stay_on_pathGoal)

        # wait-for-obstacle
        actionClientHandler = getClientHandler(p.waitForObstacleActionName, p.waitForObstacleActionTopic, wait_for_obstacleAction)
        waitForObstacleActionClient = ac.ActionClient(p.waitForObstacleActionName, actionClientHandler, p.waitForObstacleActionTopic, wait_for_obstacleAction, wait_for_obstacleGoal)

        # avoid-obstacle
        actionClientHandler = getClientHandler(p.avoidObstacleActionName, p.avoidObstacleActionTopic, avoid_obstacleAction)
        avoidObstacleActionClient = ac.ActionClient(p.avoidObstacleActionName, actionClientHandler, p.avoidObstacleActionTopic, avoid_obstacleAction, avoid_obstacleGoal)

        # trace
        actionClientHandler = getClientHandler(p.traceActionName, p.traceActionTopic, traceAction)
        traceActionClient = ac.ActionClient(p.traceActionName, actionClientHandler, p.traceActionTopic, traceAction, traceGoal)

        actionClientDic = {p.moveRobotActionName: moveRobotActionClient, p.moveToTargetActionName: moveToTargetActionClient,
                           p.stayOnPathActionName: stayOnPathActionClient, p.waitForObstacleActionName : waitForObstacleActionClient,
                           p.avoidObstacleActionName : avoidObstacleActionClient, p.traceActionName : traceActionClient}

        return actionClientDic

    except rospy.ROSException, ex:

        print "> Action client initialization failed: %s" %ex

        return None