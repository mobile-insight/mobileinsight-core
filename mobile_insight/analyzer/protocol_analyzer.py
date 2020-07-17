#!/usr/bin/python
# Filename: wcdma_rrc_analyzer.py
"""
A protocol analyzer abstraction.

Author: Yuanjie Li
"""

from .analyzer import *
from .profile import Profile, ProfileHierarchy
from .state_machine import StateMachine
import xml.etree.ElementTree as ET

__all__ = ["ProtocolAnalyzer"]


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
        self.profile = Profile(self.create_profile_hierarchy())
        self.state_machine = StateMachine(
            self.create_state_machine(),
            self.init_protocol_state)

        # update state dynamics
        self.add_source_callback(self.__update_state)

    def create_profile_hierarchy(self):
        """
        Declare a protocol-specific ProfileHierarchy

        returns: ProfileHierarchy based on protocol-specific parameters
        """
        return None

    def create_state_machine(self):
        """
        Declare a state machine

        returns: a dictinoary that describes the states and transition conditions
        """
        return None

    def init_protocol_state(self, msg):
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

    def __update_state(self, msg):
        """
        Update states in response to cellular events

        :param msg: the event (message) from the trace collector.
        """

        # Decode the message to XML format
        if not msg.data:
            return

        # FIXME: duplicate message decoding, inefficient
        log_item = msg.data.decode()
        if len(log_item) < 1:
            return
        log_item_dict = dict(log_item)

        if 'Msg' not in log_item_dict:
            return

        # Convert msg to xml format
        # log_xml = ET.fromstring(log_item_dict['Msg'])
        try:

            log_xml = ET.XML(log_item_dict['Msg'])
            xml_msg = Event(msg.timestamp, msg.type_id, log_xml)

            self.state_machine.update_state(xml_msg)
        except Exception as e:
            # Not an XML message
            return

    def get_protocol_state(self):
        """
        Returns protocol's current state

        :returns: current state_machine
        """
        return self.state_machine.get_current_state()
