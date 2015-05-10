#! /usr/bin/env python
"""
disk_logger.py

A message logger: save all traces to the disk

Author: Yuanjie Li
"""

from ..msg_filter import *
from ..msg_filter_manager import MsgFilterManager
import io

#TODO: support raw binary messages
class DiskLogger(MsgFilter):
	
	def __init__(self):
		# accept all packets
		self.filter_f = lambda msg : 1 
		# by default no loggers are enabled
		self.callback_list = []
		callback_disk_file=None	#file handler
	
	
	def callback_disk_log(self,msg):
		if self.callback_disk_file is not None:
			self.callback_disk_file.write(msg)

	#FIXME: the binary writting is not supported
	def enable_disk_log(self,filename):
		if self.callback_disk_file is not None:
			self.callback_disk_file.close()
		#TODO: check if the file has existed, otherwise it would be overwritten
		self.callback_disk_file=open(filename,'w')
		self.add_callback(self.callback_disk_log)

	def disable_disk_log(self):
		self.del_callback(self.callback_disk_log)
		self.callback_disk_file.close()


#Test functions
if __name__=="__main__":

	manager = MsgFilterManager()

	logger = DiskLogger()

	

	manager.register(logger)

	test_list=["Hello world", "Let's play games"]

	logger.enable_disk_log("test.txt")
	map(manager.onMessage, test_list)







