#! /usr/bin/env python
"""
msg_logger.py

A message logger: collect all traces

Support in-mem log and saving to disk. 
Both raw msg and plain-txt are supported.


Author: Yuanjie Li
"""

from msg_filter import MsgFilter,MsgFilterManager
import io

class MsgLogger(MsgFilter):
	
	def __init__(self):
		# accept all packets
		self.filter_f = lambda msg : 1 
		# by default no loggers are enabled
		self.callback_list = []

	#logger callbacks
	#TODO: support raw binary message
	msg_log_mem=[] # decoded message logs in memory
	def callback_mem_log(self,msg):
		self.msg_log_mem.append(msg)
		print msg
	
	callback_disk_file=None
	def callback_disk_log(self,msg):
		if self.callback_disk_file is not None:
			self.callback_disk_file.write(msg)

	def enable_mem_log(self):
		#self.msg_log_mem=[]	#clear the logs
		self.add_callback(self.callback_mem_log)

	def disable_mem_log(self):
		#self.msg_log_mem=[]	#clear the logs
		self.del_callback(self.callback_mem_log)

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

	logger = MsgLogger()

	

	manager.register(logger)

	test_list=["Hello world", "Let's play games"]

	logger.enable_mem_log()
	map(manager.onMessage, test_list)

	logger.disable_mem_log()
	map(manager.onMessage, test_list)

	logger.enable_disk_log("test.txt")
	map(manager.onMessage, test_list)







