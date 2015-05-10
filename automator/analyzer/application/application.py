#! /usr/bin/env python
"""
application.py

An abstraction for analytics application

This class should help users to
	- Automatically setup TraceManager and MsgFilterManager
	- Have a Run() function: when call it, the application runs!
The user is supposed to write one or more callback functions
	- These functions can be over ProtocolAnalyzer and/or Filters
	- Arbitrary number of parameters can be accepted
	- A easy way for user to register these callbacks 

Author: Yuanjie Li

"""

"""
Loop detection

Whenever LTE_RRC_Analyzer or LTE_ML1_Analyzer's status changes,
a loop detection algorithm should be performed and take actions. 

So an event should be triggered whenever ProtocolAnalyzer updates the states

How to make it *automatic*?
	- We cannot rely on programmers to remember adding a update function in every callback
	- Solution 1: add a special "update" function to the end of the callback list
		- In this function, all applications registered would be waken up
		- This way, whenever a new message arrives, the applications would be waken up
			- Even if this message does not change any protocol states
"""

from ..msg_filter_manager import MsgFilterManager
from ..msg_filter import *
from ..protocol_analyzer.protocol_analyzer import ProtocolAnalyzer

#For any new application (e.g., handoff loop detection), it should inherit from this abstraction

class Application:

	def __init__(self):
		#TODO: initialize MsgFilterManager and TraceManager
		self.msg_filter_manager = MsgFilterManager()
		self.callback = [] # User-defined functions

	#TODO: register ProtocolAnalyzer

	# The entrance of the application
	def run(self):
		#TODO: link the MsgFilterManager, ProtocolAnalyzer, TraceCollector, etc.



# class Application: 


# 	def __init__(self):
# 		#initialize analyzers
# 		self.lte_analyzer = LTE_RRC_Analyzer()
# 		self.ml1_analyzer = LTE_ML1_Analyzer()

# 		#The callback is triggered whenever the ProtocolAnalyzer state changes
# 		#This is a coarse-grained approach. Unnecessary triggers exist
# 		lte_analyzer.register_callback(self.loop_detection_callback)
# 		ml1_analyzer.register_callback(self.loop_detection_callback)

# 	def test_lte_status(self,ProtocolAnalyzer):
# 		return ProtocolAnalyzer


# 	# This callback embeds lte_analyzer and ml1_analyzer
# 	def loop_detection_callback(self):
# 		#detect loop. If found, change the configurations
# 		cell_list = self.lte_analyzer.get_cell_list()
# 		for i in cell_list:
# 			for j in cell_list:
# 				if ml1_analyzer.rsrp[j] > lte_analyzer.config.sib[i].ThreshHigh:
# 					send_at_cmd(...)
