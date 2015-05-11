# -*- coding: utf-8 -*-
"""
module.py

A event-driven module abstraction
Trace collector and analyzer are derived from it.

Author: Yuanjie Li
"""

__all__ = ["Event", "Module"]

# Event abstraction
class Event(object):
    def __init__(self,timestamp,type_id,data):
        self.timestamp = timestamp
        self.type_id = type_id
        self.data = data

class Module(object):

    def __init__(self):
        self.from_list={}   #module that it depends, module->callback
        self.to_list=[] #list of other module that call for this module

    def send(self,event):
        """
        Raise an event to all Analyzers in from_analyzer_list
        """
        for module in self.to_list:
            module.recv(self,event)

    def recv(self,module,event):
        """
        Upon receiving an event from module, trigger associated callbacks
        """
        pass
