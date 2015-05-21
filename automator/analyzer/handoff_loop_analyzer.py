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

		"""
			Implementation plan:
			step 1: inter-freq/RAT only in idle-state
			step 2: introduce intra-freq in idle-state
			step 3: introduce cell-individual offsets
			step 4: introduce active-state handoff
		"""

		#Get cell list and configurations
		cell_list = self.__rrc_analyzer.get_cell_list()

		#test
		for cell in cell_list:
			self.__rrc_analyzer.get_cell_config(cell).dump()

		# cell_config = {}	# each cell's configuration
		# cell_config_visited = {} #boolean matrix: [i][j] indicates if ci->cj has been visited
		# for cell in cell_list:
		# 	cell_config[cell]=self.__rrc_analyzer.get_cell_config(cell)
		# 	#TODO: from inter-freq/RAT to intra-freq, from idle-only to idle+active
		# 	inter_freq_dict=cell_config[cell].sib.inter_freq_config
		# 	neighbor_cells=[] #all neibghboring cells (including cells under same freq)
		# 	for freq in inter_freq_dict:
		# 		neighbor_cells+=self.__rrc_analyzer.get_cell_on_freq(freq)
		# 	#initially all links are marked unvisited
		# 	cell_config_visited[cell]={x:False for x in neighbor_cells}


		# #TODO: a mapping function from freq to the cell list
		
		# if cell_list:
		# 	# We implement the loop detection algorithm in Proposition 3,
		# 	# because preferences are observed to be inconsistent
		# 	dfs_stack=[cell_list.pop()]	#starting cell
		# 	#instead of real minimin meas (-140dBm), we apply relative meas here
		# 	virtual_rss=[0]	

		# 	while dfs_stack:
		# 		src_cell = dfs_stack.pop()
		# 		src_rss = virtual_rss.pop()
		# 		dst_cell = None
		# 		dst_rss = None

		# 		for cell in cell_config_visited[src_cell]:
		# 			if not cell_config_visited[src_cell][cell]:
		# 				dst_cell = cell
		# 				cell_config_visited[src_cell][dst_cell]=True
		# 				break

		# 		if dst_cell == None: #src's all neighbors have been visited
		# 			continue

		# 		dst_freq=cell_config[dst_cell].status.freq
		# 		#compare the preference
		# 		src_pref=cell_config[src_cell].sib.serv_config.priority
		# 		dst_pref=cell_config[src_cell].sib.inter_freq_config[dst_freq].priority

		# 		if src_pref<dst_pref:
		# 			threshxhigh=cell_config[src_cell].sib.inter_freq_config[dst_freq].threshx_high	
		# 			dfs_stack.append(src_cell)
		# 			dfs_stack.append(dst_cell)
		# 			dfs_stack.append(src_rss)
		# 			virtual_rss.append(threshxhigh)
		# 		elif src_pref>dst_pref:
		# 			threshserv=cell_config[src_cell].sib.serv_config.threshserv_low
		# 			if src_rss >= threshserv:	#prone the loop
		# 				dfs_stack.append(src_cell)
		# 				virtual_rss.append(src_rss)
		# 			else:
		# 				threshxlow=cell_config[src_cell].sib.inter_freq_config[dst_freq].threshx_low
		# 				dfs_stack.append(src_cell)
		# 				dfs_stack.append(dst_cell)
		# 				virtual_rss.append(src_rss)
		# 				virtual_rss.append(threshxlow)

		# # 		else: #src_pref==dst_pref
		# # 			src_rss=virtual_rss.pop()
		# # 			dst_rss=src_rss+

		# 		#Loop report
		# 		#TODO: optimize the code. Avoid list traverse 
		# 		if dfs_stack.count(dst_cell)>1:
		# 			dfs_stack.pop()
		# 			virtual_rss.pop()
		# 			loop_report="Persistent loop: "
		# 			for cell in dfs_stack:
		# 				loop_report=loop_report+cell+"->"
		# 			loop_report=loop_report+dst_cell
		# 			print loop_report



