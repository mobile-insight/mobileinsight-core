#!/usr/bin/python
# Filename: msg_logger.py
"""
A simple message dumper in std IO

Author: Yuanjie Li
"""

from analyzer import *

import xml.etree.ElementTree as ET
import io

__all__=["MsgLogger"]

class MsgLogger(Analyzer):
    """
    A simple dumper to print messages
    """
    
    def __init__(self):
        Analyzer.__init__(self)
        # a message dump has no analyzer in from/to_list
        # it only has a single callback for the source

        self.__msg_log=[] # in-memory message log
        self.add_source_callback(self.__dump_message)
        self.decode_type = 0

    def __dump_message(self,msg):
        """
        Print the received message

        :param msg: the received message
        """
        self.__msg_log.append(msg)
        print msg.timestamp,msg.type_id
        if self.decode_type == self.XML:
            print msg.data.decode_xml()
        elif self.decode_type == self.JSON:
            print msg.data.decode_json()

    #Decoding scheme

    NO_DECODING = 0
    XML = 1
    JSON = 2

    def set_decoding (self, decode_type):
        """
        Specify how to decode the messages

        :param decode_type: specify how to decode messages. It can be MsgLogger.NO_DECODING, MsgLogger.XML or MsgLogger.JSON
        :type decode_type: int
        """

        self.decode_type = decode_type


