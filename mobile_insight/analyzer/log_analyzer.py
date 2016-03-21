#!/usr/bin/python
# Filename: log_analyzer.py
"""
A simple analyzer that displays retrives a list of events from log file

"""
from analyzer import *

try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET
import io
import datetime
from mobile_insight.monitor import QmdlReplayer
from mobile_insight.monitor.dm_collector import dm_collector_c
from mobile_insight.monitor.dm_collector.dm_endec.dm_log_packet import DMLogPacket


__all__=["LogAnalyzer"]

class LogAnalyzer(Analyzer):
    """
    A simple analyzer of log events
    """
    
    def __init__(self, listener_callback):
        Analyzer.__init__(self)
        # a message dump has no analyzer in from/to_list
        # it only has a single callback for the source

        self.msg_logs=[] # in-memory message log
        self.src = QmdlReplayer()
        self.set_source(self.src)
        self.add_source_callback(self.__dump_message)
        self.listener_callback = listener_callback

        self.supported_types = set(dm_collector_c.log_packet_types)
        for st in self.supported_types:
            self.src.enable_log(st)

    def AnalyzeFile(self, fileName):
        self.msg_logs = []
        self.src.set_input_path(fileName)
        self.src.run()
        if self.listener_callback:
            self.listener_callback()



    def __dump_message(self,msg):
        """
        Print the received message

        :param msg: the received message
        """
        payload = msg.data.decode_xml()
        timestamp_beg = payload.find("timestamp\">") + 11
        timestamp_end = payload.find("</", timestamp_beg)
        msg_timestamp = payload[timestamp_beg:timestamp_end]
        self.msg_logs.append({
            'Timestamp' : msg_timestamp,
            'TypeID' : msg.type_id,
            'Payload' : payload
            })