#!/usr/bin/python
# Filename: wcdma_rrc_analyzer.py
"""
A protocol analyzer abstraction.

Author: Yuanjie Li
"""

from analyzer import *
from profile import Profile,ProfileHierarchy
from state_machine import StateMachine

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
        self.__profile = Profile(self.__create_profile_hierarchy())
        self.__state_machine = StateMachnie(self.__create_state_machine())

        #Update state dynamics
        self.add_source_callback(self.__update_state)
      

    def __create_profile_hierarchy(self):
        """
        Declare a protocol-specific ProfileHierarchy

        returns: ProfileHierarchy based on protocol-specific parameters
        """
        return None

    def __create_state_machine(self):
        """
        Declare a state machine

        returns: ProfileHierarchy based on protocol-specific FSMs
        """
        return None

    def __init_protocol_state(self):
        """
        At bootstrap, determine the protocol's current state

        :returns: a string of current state
        """
        return None

    def declare_failure_flag(self):
        """
        Initialize failure flags
        """
        pass

    def __update_state(self,msg):
        """
        Update states in response to cellular events

        :param msg: the event (message) from the trace collector.
        """
        
        #Decode the message to XML format

        #FIXME: duplicate message decoding, inefficient
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)

        #Convert msg to xml format
        log_xml = ET.fromstring(log_item_dict['Msg'])
        xml_msg = Event(msg.timestamp,msg.type_id,log_xml)

        self.__state_machine.update_state(xml_msg)

    def get_protocol_state(self):
        """
        Returns protocol's current state

        :returns: current state_machine
        """
        return self.__state_machine.get_current_state()

