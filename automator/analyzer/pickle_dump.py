# -*- coding: utf-8 -*-
"""
pickle_dump.py

A pickle message dump analyzer

Author: Jiayao Li
"""

__all__ = ["PickleDump"]

import pickle

from analyzer import *


class PickleDump(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)
        # a message dump has no analyzer in from/to_list
        # it only has a single callback for the source
        self.__msg_log = [] # in-memory message log
        self.add_source_callback(self.__dump_message)

    def set_output_path(self, path):
        self.__output_path = path
        self.__output_file = open(path, "wab")

    def __dump_message(self,msg):
        self.__msg_log.append(msg)
        pickle.dump(msg, self.__output_file)
        self.__output_file.flush()
