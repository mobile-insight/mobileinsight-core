#! /usr/bin/env python
"""
msg_dump.py

A simple message dump analyzer

Author: Yuanjie Li
"""

from analyzer import *

import xml.etree.ElementTree as ET
import io

__all__=["MsgDump"]

class MsgDump(Analyzer):

	def __dump_message(self,msg):
		self.__msg_log.append(msg)
		print msg.timestamp,msg.type_id
		
	def __init__(self):
		Analyzer.__init__(self)
		# a message dump has no analyzer in from/to_list
		# it only has a single callback for the source
		self.__msg_log=[] # in-memory message log
		self.add_source_callback(self.__dump_message)
