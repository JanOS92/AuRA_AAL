#!/usr/bin/env python

# ==============================================================================
# @file AMiRoA_fsm.py
# @brief Provides the finite state machine for controlling the AuRA implementation
# @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
# ==============================================================================

## Imports

# include directories
import sys
sys.path.append('./types')
sys.path.append('./interfaces')

# Utils
import sys, signal

# Interfaces
import action_interface as ai

# Types
import enums as e

# States
import states as s

# Parameters
import parameters as p

# ROS-SMACH
import smach
from smach import StateMachine, Concurrence
import smach_ros

# ROS
import rospy

## Global variables
# Values
TimeOut_s = rospy.Duration(10,0)
SleepTime_s = 3
targetId = e.Targets.ASSEMBLY2.value

## Functions

## Description:
# Allow "Ctrl+C" exit
## Parameter:
# -
## Return
# -
def signal_handler(signal, frame):

    sys.exit(0)

# main
def main() :

    signal.signal(signal.SIGINT, signal_handler)

    rospy.init_node('AMiRoA_state_machine')

    # initialize the action interface and get the client references
    actionClientDic = ai.initializeActionInterface()

    amiroa_sm = smach.StateMachine(outcomes = ['succeeded','failed']) # create the SMACH state machine

    with amiroa_sm: # open the container
    
        StateMachine.add(p.SleepStateName, s.SleepState(p.sleepTime), transitions = {'done' : p.InitializationStateName})
        
        StateMachine.add(p.InitializationStateName, s.InitializationState(), transitions = {'done' : p.NavigateToGatewayStateName, 'fail' : 'failed'})

        StateMachine.add(p.NavigateToGatewayStateName, s.NavigateToGatewayState(targetId, actionClientDic), transitions = {'gateway' : p.NavigateToDeflectorStateName, 'fail' : 'failed'})

        StateMachine.add(p.NavigateToDeflectorStateName, s.NavigateToDeflectorState(targetId, actionClientDic), transitions = {'transit' : p.NavigateToDeflectorStateName, 'descend' : p.NavigateToGoalStateName, 'fail' : 'failed'})

        StateMachine.add(p.NavigateToGoalStateName, s.NavigateToGoalState(targetId, actionClientDic), transitions = {'goal' : 'succeeded','gateway': p.NavigateToDeflectorStateName, 'fail' : 'failed'})

    sis = smach_ros.IntrospectionServer('smach_amiroa', amiroa_sm, '/AMIROA') # attach a SMACH introspection server

    sis.start() # start introspection server
    
    outcome = amiroa_sm.execute() # execute SMACH plan

    ai.deinstantiateAllActions(actionClientDic) # deinstantiate all motor schemas

    rospy.signal_shutdown('success')

if __name__ == '__main__':
    main()