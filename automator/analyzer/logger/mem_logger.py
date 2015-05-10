#! /usr/bin/env python
"""
mem_logger.py

An in-mem message logger: collect all traces in the memory


Author: Yuanjie Li
"""

from ..msg_filter import *
from ..msg_filter_manager import MsgFilterManager
import xml.etree.ElementTree as ET
import io

class MemLogger(MsgFilter):
	
	def __init__(self):
		# accept all packets
		self.filter_f = lambda msg : 1 
		# by default no loggers are enabled
		self.callback_list = [self.callback_mem_log]

	#logger callbacks
	#TODO: support raw binary message
	msg_log_mem=[] # decoded message logs in memory
	def callback_mem_log(self,msg):
		self.msg_log_mem.append(msg)
		print msg.timestamp,msg.type_id
		ET.dump(msg.data)

	

#Test functions
if __name__=="__main__":

	manager = MsgFilterManager()

	logger = MemLogger()

	

	manager.register(logger)

	test_list=["Hello world", "Let's play games"]

	map(manager.onMessage, test_list)
