#!/usr/bin/python
# Filename: msg_serializer.py
"""
A serializer of mobile network messages. It can be used for offline analysis and replay-based debugging.

Author: Jiayao Li
"""

__all__ = ["MsgSerializer"]

import pickle

from .analyzer import *


class MsgSerializer(Analyzer):
    """
    An ananlyzer to save the message to disk.
    It can be used for replay-based debugging and offline analysis.
    """

    def __init__(self):
        Analyzer.__init__(self)
        # a message dump has no analyzer in from/to_list
        # it only has a single callback for the source
        self.__msg_log = []  # in-memory message log
        self.add_source_callback(self.__dump_message)

    def set_output_path(self, path):
        """
        Set the file to save the messages.

        :param path: the file path to save the messages
        :type path: string
        """
        self.__output_path = path
        self.__output_file = open(path, "wb")

    def __dump_message(self, msg):
        """
        A callback function to save message to the disk

        :param msg: the received event (message)
        :type msg: Event
        """
        self.__msg_log.append(msg)
        pickle.dump(msg, self.__output_file)
        self.__output_file.flush()
