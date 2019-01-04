#!/usr/bin/env python

# ==============================================================================
# @file enums.py
# @brief Provides the class definition and declaration for "ActionClient"
# @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
# ==============================================================================

class ActionClient():

    # constructor
    def __init__(self, name, clientHandler, actionTopic, actionHandler, goalHandler):

        # public attributes
        # protected attributes
        # private attributes
        self.__name = name
        self.__clientHandler = clientHandler
        self.__actionTopic = actionTopic
        self.__actionHandler = actionHandler
        self.__goalHandler = goalHandler

    # getter
    def getName(self):
        return self.__name
    def getClientHandler(self):
        return self.__clientHandler
    def getActionTopic(self):
        return self.__actionTopic
    def getActionHandler(self):
        return self.__actionHandler
    def getGoalHandler(self):
        return self.__goalHandler