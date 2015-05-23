#! /usr/bin/env python
"""
analyzer.py

A event-driven analyzer abstraction, 
including low-level msg filter and high-level analyzer

Author: Yuanjie Li
"""

from element import Element, Event

class Analyzer(Element):

	def __init__(self):
		Element.__init__(self)
		self.source=None	#trace source collector
		#callback when source pushes messages
		#FIXME: looks redundant with the from_list
		self.source_callback=[]	

	# Set the trace source collector
	# Both this analyzer and other analyzers it depends would set to this source
	# FIXME: current implementation is dangerous, any from/to analyzer can change source!
	# FIXME: if looped analyzer dependency exists, dead lock happens here!!!
	def set_source(self,source):
		"""
			Set the trace source
			Bottom-up setting: the included analyzers should be evaluated first,
			then top analyzer
		"""
		#Recursion for analyzers it depends on
		for analyzer in self.from_list:
			analyzer.set_source(source)
			
		if self.source != None:
			self.source.deregister(self)
		self.source = source
		source.register(self)

	def add_source_callback(self,callback):
		if callback not in self.source_callback:
			self.source_callback.append(callback)

	def rm_source_callback(self,callback):
		if callback in self.source_callback:
			self.source_callback.remove(callback)

	# receive events from analyzer, then trigger functions in callback_list
	def include_analyzer(self,analyzer,callback_list):
		#WARNING: if analyzer exits, its callback_list would be overwritten!!!
		self.from_list[analyzer]=callback_list
		#Add the analyzer to the to_list
		if self not in analyzer.to_list:
			analyzer.to_list.append(self)

	def exclude_analyzer(self,analyzer):
		if self in analyzer.to_list:
			analyzer.to_list.remove(self)
		del self.from_list[analyzer]

	def recv(self,module,event):
		if module==self.source:
			for f in self.source_callback:
				f(event)
		else:
			for f in self.from_list[module]:
				f(event)
