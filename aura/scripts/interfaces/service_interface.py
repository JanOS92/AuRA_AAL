#!/usr/bin/env python

# ==============================================================================
# @file enums.py
# @brief Provides a meta interface for the service communication
# @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
# ==============================================================================

## Imports

# include directories
import sys
sys.path.append('../types')

# Types
import parameters as p

# ROS
import rospy

# ROS: Service
from aal_potential_field_navigation.srv import initiate_image_routine

## Interface

## Description:
# Call the service which triggers the initial image routine
## Parameter:
# -
## Return
# True/False
def callInitialImageRoutine():

    print "calling image routine..."

    try:

        rospy.wait_for_service(p.initialImageRoutineServiceTopic)

        initiateImageRoutineService = rospy.ServiceProxy(p.initialImageRoutineServiceTopic, initiate_image_routine)

        resp = initiateImageRoutineService()

        rospy.wait_for_service(p.initialImageRoutineServiceTopic) # wait for response

    except rospy.ServiceException, e:

        print "Image routine failed: %s"%e

    if resp.jobDone_i:

        print("initial image routine successfully finished...")

        return True

    else:

        return False
