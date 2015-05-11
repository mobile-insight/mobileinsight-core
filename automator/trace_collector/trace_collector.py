#! /usr/bin/env python
"""
trace_collector.py

An abstraction for trace collector.
All specific trace collectors (QXDM, RIL, 802.11, pcap) derive from it

Author: Yuanjie Li
"""

from module import *

class TraceCollector(Module):

    def __init__(self):
        self.from_list={} # No source for TraceCollector
        self.to_list=[]


    # Add an analyzer that needs the message
    def register(self,analyzer):
        if analyzer not in self.to_list:
            self.to_list.append(analyzer)

    def deregister(self,analyzer):
        if analyzer in self.to_list:
            self.to_list.remove(analyzer)

    def run(self):
        """
        Start collecting the traces.
        send() is used to pass message to analyzers
        EVERY derived collector SHOULD overload this function to generate msg (event) 
        Currently only QXDM is used, and each time only ONE collector can be used
        """
        # TODO: we may extend this module to support multiple sources later
        pass
