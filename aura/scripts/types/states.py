#!/usr/bin/env python

# ==============================================================================
# @file enums.py
# @brief Provides the definition of custom SMACH states
# @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
# ==============================================================================

## Imports

# include directories
import sys
sys.path.append('../interfaces')

# Enums
import enums as e

# Parameters
import parameters as p

# Interfaces
import action_interface as ai
import service_interface as si

# SMACH
import smach
from smach import StateMachine, Concurrence
import smach_ros

# Utils
import time

# ROS
import rospy

## State definition

# define state "SleepState"
# source: https://raw.githubusercontent.com/felix-kolbe/uashh-rvl-ros-pkg/master/uashh_smach/src/uashh_smach/util.py
class SleepState(smach.State):
    """Sleep for a time duration, given either on initialization or via userdata.

    duration: of type rospy Duration or float in seconds. If not given or None,
                duration is read from userdata key 'duration'.

    userdata input duration: of type rospy Duration or float in seconds (not
                registered as input key if given on initialization)
    """
    def __init__(self, duration = None):

        smach.State.__init__(self, outcomes = ['done'],
                             input_keys = ['duration'] if duration is None else [])

        self.duration = duration

    def execute(self, userdata):

        duration = userdata.duration if self.duration is None else self.duration

        rospy.loginfo("SleepState sleeping for %d seconds" % duration)

        # sleep in steps to handle state preemption
        SLEEP_STEP = 2 # maximum to sleep per step

        while duration > 0:

            sleeptime = SLEEP_STEP if duration > SLEEP_STEP else duration

            duration -= sleeptime

            rospy.sleep(sleeptime)

        return 'done'

#define state: "Initialization"
class InitializationState(smach.State):

    def __init__(self):

        smach.State.__init__(self, outcomes = ['done','fail'])

        # public attributes
        self.counter = 0
        # protected attributes
        # private attributes

    def execute(self, userdata):

        if si.callInitialImageRoutine():

            return 'done'

        else:

            return 'fail'

# define state: "Navigate to gateway"
class NavigateToGatewayState(smach.State):

    def __init__(self, targetId, actionClientDic):

        smach.State.__init__(self, outcomes = ['gateway','fail'])

        # public attributes
        self.counter = 0
        # protected attributes
        # private attributes
        self.__done = False
        self.__targetId = targetId
        self.__actionClientDic = actionClientDic

    # feedback callback
    def navigateToGatewayFeedbackCb(self, move_to_targetFeedback):

        if move_to_targetFeedback.RFIDAction_i == e.RFIDAction.ASCEND.value:

            self.__done = True

            return

    # specific wait method
    def wait_until(self, period = p.period):

        while True:

            if self.__done:

                self.__done = False

                return True

            elif rospy.is_shutdown():

                break

            else: time.sleep(period)

    def execute(self, userdata):

        try:

            # call move-to-target
            moveToTargetActionClient = self.__actionClientDic[p.moveToTargetActionName]
            goalBuffer = moveToTargetActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.MOVETOTARGET.value
            goalBuffer.finalTargetRFID_ui = self.__targetId
            goalBuffer.checkJobLogic_i = e.JobLogic.DEFAULT.value
            moveToTargetActionClient.getClientHandler().send_goal(goalBuffer, feedback_cb = self.navigateToGatewayFeedbackCb)

            # call stay_on_path
            stayOnPathActionClient = self.__actionClientDic[p.stayOnPathActionName]
            goalBuffer = stayOnPathActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.STAYONPATH.value
            goalBuffer.finalTargetRFID_ui = self.__targetId
            stayOnPathActionClient.getClientHandler().send_goal(goalBuffer)

            # call wait_for_obstacle
            waitForObstacleActionClient = self.__actionClientDic[p.waitForObstacleActionName]
            goalBuffer = waitForObstacleActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.WAITFOROBSTACLE.value
            waitForObstacleActionClient.getClientHandler().send_goal(goalBuffer)

            # call move_robot
            moveRobotActionClient = self.__actionClientDic[p.moveRobotActionName]
            goalBuffer = moveRobotActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.MOVEROBOT.value
            moveRobotActionClient.getClientHandler().send_goal(goalBuffer)

            # call trace (visualization)
            # traceActionClient = self.__actionClientDic[p.traceActionName]
            # goalBuffer = traceActionClient.getGoalHandler()
            # goalBuffer.doTracing = True
            # traceActionClient.getClientHandler().send_goal(goalBuffer)

        except rospy.ROSException, ex:

            print "Navigate to gateway failed: %s"%ex

        # wait here
        if self.wait_until():

            return 'gateway'

        else:

            return 'fail'

# define state: "Navigate to deflector"
class NavigateToDeflectorState(smach.State):

    def __init__(self, targetId, actionClientDic):

        smach.State.__init__(self, outcomes = ['transit','descend','fail'])
        # public attributes
        self.counter = 0
        # protected attributes
        # private attributes
        self.__done = False
        self.__targetId = targetId
        self.__actionClientDic = actionClientDic
        self.__rfidAction = None

    # specific wait method
    def wait_until(self, period = p.period):

        while True:

            if self.__done:

                self.__done = False

                return True

            elif rospy.is_shutdown():

                break

            else: time.sleep(period)

    # feedback callback
    def navigateToDeflectorFeedbackCb(self, move_to_targetFeedback):

        if move_to_targetFeedback.RFIDAction_i == e.RFIDAction.DESCEND.value or move_to_targetFeedback.RFIDAction_i == e.RFIDAction.TRANSIT.value:

            self.__done = True

            self.__rfidAction = None # clear the value

            self.__rfidAction = move_to_targetFeedback.RFIDAction_i

            return

    def execute(self, userdata):

        # deinstantiate unused motor schemas
        waitForObstacleActionClient = self.__actionClientDic[p.waitForObstacleActionName]
        ai.deinstantiateAction(waitForObstacleActionClient)

        try:

            # call move_to_target
            moveToTargetActionClient = self.__actionClientDic[p.moveToTargetActionName]
            goalBuffer = moveToTargetActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.MOVETOTARGET.value
            goalBuffer.finalTargetRFID_ui = self.__targetId
            goalBuffer.checkJobLogic_i = e.JobLogic.DEFAULT.value
            moveToTargetActionClient.getClientHandler().send_goal(goalBuffer, feedback_cb = self.navigateToDeflectorFeedbackCb)

            # call stay_on_path
            stayOnPathActionClient = self.__actionClientDic[p.stayOnPathActionName]
            goalBuffer = stayOnPathActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.STAYONPATH.value
            goalBuffer.finalTargetRFID_ui = self.__targetId
            stayOnPathActionClient.getClientHandler().send_goal(goalBuffer)

            # call avoid_obstacle
            # avoidObstacleActionClient = self.__actionClientDic[p.avoidObstacleActionName]
            # goalBuffer = avoidObstacleActionClient.getGoalHandler()
            # goalBuffer.id_ui = e.MotorSchemaIds.AVOIDOBSTACLE.value
            # avoidObstacleActionClient.getClientHandler().send_goal(goalBuffer)

        except rospy.ROSException, ex:

            print "Navigate to gateway failed: %s"%ex

        # wait here
        self.wait_until()

        if self.__rfidAction == e.RFIDAction.TRANSIT.value:

            return 'transit'

        elif self.__rfidAction == e.RFIDAction.DESCEND.value:

            return 'descend'

        else:

            return 'fail'

# define state: "Navigate to goal"
class NavigateToGoalState(smach.State):

    def __init__(self, targetId, actionClientDic):

        smach.State.__init__(self, outcomes = ['goal','gateway','fail'])

        # public attributes
        self.counter = 0
        # protected attributes
        # private attributes
        self.__done = False
        self.__targetId = targetId
        self.__actionClientDic = actionClientDic
        self.__rfidAction = None

    # specific wait method
    def wait_until(self, period = p.period):

        while True:

            if self.__done:

                self.__done = False

                return True

            elif rospy.is_shutdown():

                break

            else: time.sleep(period)

    # feedback callback
    def navigateToGoalFeedbackCb(self, move_to_targetFeedback):

        if move_to_targetFeedback.RFIDAction_i == e.RFIDAction.STOP.value or move_to_targetFeedback.RFIDAction_i == e.RFIDAction.ASCEND.value:

            self.__done = True

            self.__rfidAction = None # clear the value

            self.__rfidAction = move_to_targetFeedback.RFIDAction_i

            return

    def execute(self, userdata):

        global moveToTargetActionClient, stayOnPathActionClient, RFIDActionCallbackValue

        # deinstantiate unused motor schemas
        avoidObstacleActionClient = self.__actionClientDic[p.avoidObstacleActionName]
        ai.deinstantiateAction(avoidObstacleActionClient)

        try:

            # call move_to_target
            moveToTargetActionClient = self.__actionClientDic[p.moveToTargetActionName]
            goalBuffer = moveToTargetActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.MOVETOTARGET.value
            goalBuffer.finalTargetRFID_ui = self.__targetId
            goalBuffer.checkJobLogic_i = e.JobLogic.GOAL.value
            moveToTargetActionClient.getClientHandler().send_goal(goalBuffer, feedback_cb = self.navigateToGoalFeedbackCb)

            # call stay_on_path
            stayOnPathActionClient = self.__actionClientDic[p.stayOnPathActionName]
            goalBuffer = stayOnPathActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.STAYONPATH.value
            goalBuffer.finalTargetRFID_ui = self.__targetId
            stayOnPathActionClient.getClientHandler().send_goal(goalBuffer)

            # call wait_for_obstacle
            waitForObstacleActionClient = self.__actionClientDic[p.waitForObstacleActionName]
            goalBuffer = waitForObstacleActionClient.getGoalHandler()
            goalBuffer.id_ui = e.MotorSchemaIds.WAITFOROBSTACLE.value
            waitForObstacleActionClient.getClientHandler().send_goal(goalBuffer)

        except rospy.ROSException, ex:

            print "Navigate to gateway failed: %s"%ex

        # wait here
        self.wait_until()

        # deinstantiate trace
        # traceActionClient = self.__actionClientDic[p.traceActionName]
        # ai.deinstantiateAction(traceActionClient)

        if self.__rfidAction == e.RFIDAction.ASCEND.value:

            return 'gateway'

        elif self.__rfidAction == e.RFIDAction.STOP.value:

            return 'goal'

        else:

            return 'fail'