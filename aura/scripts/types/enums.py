#!/usr/bin/env python

# ==============================================================================
# @file enums.py
# @brief Provides the definition of some required enum types
# @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
# ==============================================================================

## Imports

from enum import Enum

class RFIDAction(Enum):

    NOTHING = 0
    START = 1
    STOP = 2
    ASCEND = 3
    DESCEND = 4
    TRANSIT = 5

    def __new__(cls, value):

        member = object.__new__(cls)
        member._value_ = value
        return member

    def __int__(self):

        return self.value

class Targets(Enum):

    STOCK1 = 1000
    STOCK2 = 2000
    TESTING = 100
    ASSEMBLY1 = 10
    ASSEMBLY2 = 20

    def __new__(cls, value):

        member = object.__new__(cls)
        member._value_ = value
        return member

    def __int__(self):

        return self.value

class MotorSchemaIds(Enum):

    MOVEROBOT = 1
    MOVETOTARGET = 2
    STAYONPATH = 3
    WAITFOROBSTACLE = 4
    AVOIDOBSTACLE = 5

    def __new__(cls, value):

        member = object.__new__(cls)
        member._value_ = value
        return member

    def __int__(self):

        return self.value

class JobLogic(Enum):

    DEFAULT = 0
    GOAL = 1

    def __new__(cls, value):

        member = object.__new__(cls)
        member._value_ = value
        return member

    def __int__(self):

        return self.value