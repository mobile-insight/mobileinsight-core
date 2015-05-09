#! /usr/bin/env python
"""
msg_filter.py

An abstraction for message filter and callbacks

Author: Yuanjie Li
"""

# Tutorial
# Step 0: Create a Msg_filter_manager
# Step 1: Create a msg_filter: filtering condition + callback
# Step 2: Register the msg_filter to the Msg_filter_manager
# Step 3: link the Msg_filter_manager to the dissector


#Base class for the per-message filter
#TODO: LTE_Msg_filter, WCDMA_Msg_filter, RRC_Msg_filter
#TODO: Optimize the performance afterwards
#TODO: define various Msg_filters in specific types
#TODO: Analyzer abstraction that has one callback and multiple filters (e.g., RRC+ML1)
class MsgFilter:

	filter_f = lambda msg: 1	# filter function. By default any packets are accepted
	callback_list = []	# callback function list 

	def __init__(self,filter_f,callback_list):
		self.filter_f = filter_f
		self.callback_list = callback_list

	def add_callback(self,callback):
		if callback not in self.callback_list:
			self.callback_list.append(callback)

	def del_callback(self,callback):
		if callback in self.callback_list:
			self.callback_list.remove(callback)

	def set_filter(self,f):
		self.filter_f = f