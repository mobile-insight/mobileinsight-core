#!/usr/bin/python
# Filename: nr_dci_analyzer.py
"""
A NR 5G DCI analyzer.
Author: 
"""

from mobile_insight.analyzer.analyzer import *
import json

__all__ = ["NrDciAnalyzer"]


class NrDciAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

    def set_source(self, source):
        """
        Set the trace source. 
        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("NR_DCI_Message")
    

    def __msg_callback(self, msg):
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)


        #if the type is not JSON serializable (like datatime), convert it to str
        json_log = json.dumps(log_item_dict, indent=4, default=str) 
        print(json_log)