#! /usr/bin/env python
"""
lte_ue_analyzer.py

An analyzer for LTE user's internal events

Author: Yuanjie Li
"""

from analyzer import *

import matplotlib.pyplot as plt
import matplotlib.animation as animation


class lte_ue_analyzer(Analyzer):

	def __init__(self):

		Analyzer.__init__(self)

		#init packet filters
		self.add_source_callback(self.ue_event_filter)

		#init figure
		self.fig = plt.figure()


	def set_source(self,source):
		Analyzer.set_source(self,source)
		#enable user's internal events
		source.enable_log("LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results")

	def ue_event_filter(self,msg):
		#TODO: support more user events
		self.serving_cell_rsrp(msg)


	def serving_cell_rsrp(self,msg):
		if msg.type_id == "LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results":
			msg_dict=dict(msg.data)
			print msg.timestamp,msg_dict['Serving Filtered RSRP(dBm)']

