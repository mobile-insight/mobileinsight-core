#!/usr/bin/python
# Filename: monitor.py
"""
An abstraction for mobile network monitor.
All specific trace collectors (DM, RIL, pcap, Replayer, etc.) derive from it.

Author: Yuanjie Li
"""

from element import Element, Event

class Monitor(Element):
    '''
    An abstraction for mobile network monitors
    '''

    def __init__(self):
        # No source for Monitor
        Element.__init__(self)


    # Add an analyzer that needs the message
    def register(self,analyzer):
        """
        Register an analyzer driven by this monitor

        :param analyzer: the analyzer to be added
        :type analyzer: Analyzer
        """
        if analyzer not in self.to_list:
            self.to_list.append(analyzer)

    def deregister(self,analyzer):
        """
        Deregister an analyzer driven by this monitor

        :param analyzer: the analyzer to be removed
        :type analyzer: Analyzer
        """
        if analyzer in self.to_list:
            self.to_list.remove(analyzer)

    def enable_log(self, type_name):
        """
        Enable the messages to be monitored.

        :param type_name: the message type to be monitored
        :type type_name: string
        """
        pass

    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.
        """
        #send() is used to pass message to analyzers
        #EVERY derived collector SHOULD overload this function to generate msg (event) 
        #Currently only QXDM is used, and each time only ONE collector can be used
        # TODO: we may extend this module to support multiple sources later
        pass
