#!/usr/bin/python
# Filename: modem_debug_analyzer.py
"""
A debugger for cellular interface

Author: Yuanjie Li
"""


from mobile_insight.analyzer.analyzer import *

__all__ = ["ModemDebugAnalyzer"]


class ModemDebugAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("Modem_debug_message")

    def __msg_callback(self, msg):

        if msg.type_id == "Modem_debug_message":

            log_item = msg.data.decode()

            if 'Msg' in log_item:
                self.log_info(log_item["Msg"])
