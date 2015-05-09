#! /usr/bin/env python
"""
msg_filter_manager.py

A centralized manager for message filter and callbacks

Author: Yuanjie Li
"""

from . import msg_filter

# Abstraction for msg_filter managment. 
# Accept messgesa from the dissector
# Trigger the filter and corresponding callbacks
# TODO: bind the Msg_filter_manager to the message traces
class MsgFilterManager:
	# A list of all registered msg_filter
	msg_filter_list=[]

	def __init__(self):
		self.msg_filter_list = []

	def register(self, msg_filter):
		if msg_filter not in self.msg_filter_list:
			self.msg_filter_list.append(msg_filter)

	def deregister(self, msg_filter):
		if msg_filter in self.msg_filter_list:
			self.msg_filter_list.remove(msg_filter)

	# a decoded message from the dissector
	# msg is a list of [timestamp,type_id,log_item]
	def onMessage(self, msg):	
		
		print "onMessage: %s" % msg
		#test if the msg satisfies the filter
		f = lambda x: x.filter_f(msg) 
		# Find all filters looking for msg
		filtered_list = filter(f,self.msg_filter_list)

		# For each msg_filter, trigger the call_back
		g = lambda x: map(lambda ff:ff(msg),x.callback_list)
		# Apply to all msg_filters
		map(g,filtered_list)



#Test functions
if __name__ == "__main__":

	manager = MsgFilterManager()

	def callback_1(x):
		print x
	def callback_2(x):
		print x*x

	f = callback_1
	g = callback_2
	filter1 = MsgFilter(lambda x:1,[f,g])	#accept all messages
	filter2 = MsgFilter(lambda x:x%2, [f])



	manager.register(filter1)
	manager.register(filter2)

	test_list=[1,2,3,4,5]
	map(manager.onMessage, test_list)
	print ""

	manager.deregister(filter1)
	map(manager.onMessage, test_list)
	print ""

	filter2.add_callback(g)
	map(manager.onMessage, test_list)
	print ""

	filter2.del_callback(f)
	map(manager.onMessage, test_list)
