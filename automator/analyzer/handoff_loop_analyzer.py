#! /usr/bin/env python
"""
handoff_loop_analyzer.py

An analyzer for handoff loop detection and resolution

Author: Yuanjie Li
"""
import xml.etree.ElementTree as ET
from analyzer import *
from rrc_analyzer import RrcAnalyzer

class HandoffLoopAnalyzer(Analyzer):

	def __init__(self):

		Analyzer.__init__(self)

		#include analyzers
		#V1: RrcAnalyzer only
		#V2: RrcAnalyzer + UeAnanlyzer

		self.__rrc_analyzer = RrcAnalyzer()

		self.include_analyzer(self.__rrc_analyzer,[self.__loop_detection])

	def __loop_detection(self,msg):
		"""
			Detection persistent loops, configure device to avoid it
		"""
		if msg.type_id!="RrcAnalyzer":
			return


		print "\n\nLoop Detection:"
		cell_list = self.__rrc_analyzer.get_cell_list()
		print cell_list

		for cell in cell_list:
			self.__rrc_analyzer.get_cell_config(cell).dump()

		# We implement the loop detection algorithm in Proposition 3,
		# because preferences are observed to be inconsistent

