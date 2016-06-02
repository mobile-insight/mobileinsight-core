#!/usr/bin/python
# Filename: element.py
"""
Basic abstractions for the trace collector and analyzer.

Author: Yuanjie Li
"""

__all__ = ["Event", "Element"]


class Event(object):
    '''The event is used to trigger the analyzer and perform some actions.

    The event can be raised by a trace collector (a message) or an analyzer.
    An event is a triple of (timestamp, type, data).
    '''
    def __init__(self,timestamp,type_id,data):
        self.timestamp = timestamp
        self.type_id = type_id
        self.data = data

class Element(object):
    '''The parent class to derive trace collectors and analyzers.
    '''

    total_count=0

    def __init__(self):
        self.from_list={}   #module that it depends, module->callback
        self.to_list=[] #list of other module that call for this module

    def send(self,event):
        """
        Raise an event to all Analyzers in from_analyzer_list

        :param event: the event to be sent
        """
        # A lambda function: input as a callback, output as passing event to this callback
        G = lambda module: module.recv(self,event)
        self.total_count = self.total_count+1
        print event.type_id, str(len(self.to_list)), str(self.total_count)
        map(G, self.to_list)

    def recv(self,module,event):
        """
        Upon receiving an event from module, trigger associated callbacks

        This method should be overwritten by the analyzer and trace collector

        :param module: the module who raises the event
        :param event: the event to be received
        """
        pass
