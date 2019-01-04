#!/usr/bin/env python

# ==============================================================================
# @file enums.py
# @brief Provides the declaration of parameters
# @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
# ==============================================================================

## Imports

# ROS
import rospy

## Parameters

# Service topics
initialImageRoutineServiceTopic = '/image_converting_node/initiate_image_routine/'

# Action names
moveRobotActionName = "move-robot"
moveToTargetActionName = "move-to-target"
stayOnPathActionName = "stay-on-path"
waitForObstacleActionName = "wait-for-obstacle"
avoidObstacleActionName = "avoid-obstacle"
traceActionName = "trace"

# Action topics
moveRobotActionTopic = '/move_robot/call_move_robot'
moveToTargetActionTopic = '/move_to_target/call_move_to_target'
stayOnPathActionTopic = '/stay_on_path/call_stay_on_path'
waitForObstacleActionTopic = '/wait_for_obstacle/call_wait_for_obstacle'
avoidObstacleActionTopic = '/avoid_obstacle/call_avoid_obstacle'
traceActionTopic = '/trace_node/call_trace'

# State Names
SleepStateName = 'Sleep'
InitializationStateName = 'initialization'
NavigateToGatewayStateName = 'navigate to gateway'
NavigateToDeflectorStateName = 'navigate to deflector'
NavigateToGoalStateName = 'navigate to goal'

# constant values
sleepTime = 3 # s
period = 0.25 # s
clientTimeOutDuration = rospy.Duration(10.0) # s