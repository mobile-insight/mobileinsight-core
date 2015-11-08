#!/usr/bin/python
# Filename: wcdma_rrc_analyzer.py
"""
A protocol analyzer abstraction.

Author: Yuanjie Li
"""

from analyzer import *
from profile import Profile,ProfileHierarchy
from state_machine import StateMachnie

__all__=["ProtocolAnalyzer"]


class ProtocolAnalyzer(Analyzer):

    """
    A protocol analyzer abstraction. 
    Compared with basic analyzer, ProtocolAnalyzer has three built-in functions

        - State machine dynamics

        - Profile

        - Failure flags
    """


    def __init__(self):

        """
        Initialization. 
        Each protocol should provide three initialization methods

            - Initialize ProfileHierarchy
            - Initialize StateMachnie
            - Initialize Failure flags
        """

        Analyzer.__init__(self)
        