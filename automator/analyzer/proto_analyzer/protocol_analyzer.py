#! /usr/bin/env python
"""
protocol_analyzer.py

An abstract class for protocol analyzer

Author: Yuanjie Li
"""

from ..msg_filter import *

class ProtocolAnalyzer(MsgFilter):

	def __init__(self):
		self.filter_f = lambda msg: True
		self.callback_list = []

	#TODO: add application regsiter mechanisms 

