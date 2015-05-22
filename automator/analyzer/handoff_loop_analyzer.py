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
		cell_visited = {x:False for x in cell_list} # mark if a cell has been visited

		cell_config = {}	# each cell's configuration
		cell_config_visited = {} #boolean matrix: [i][j] indicates if ci->cj has been visited
		for cell in cell_list:
			cell_config[cell]=self.__rrc_analyzer.get_cell_config(cell)

			#test
			cell_config[cell].dump()
			
			neighbor_cells=[] 
			#add neighbors under the same frequency
			neighbor_cells+=self.__rrc_analyzer.get_cell_on_freq(cell_config[cell].status.freq)
			neighbor_cells.remove(cell)	#remove the current cell itself

			#add neighbors under different frequencies
			inter_freq_dict=cell_config[cell].sib.inter_freq_config
			
			for freq in inter_freq_dict:
				neighbor_cells+=self.__rrc_analyzer.get_cell_on_freq(freq)
			
			#initially all links are marked unvisited
			cell_config_visited[cell]={x:False for x in neighbor_cells}

		
		if cell_list:

			# We implement the loop detection algorithm in Proposition 3,
			# because preferences are observed to be inconsistent
			while False in cell_visited.itervalues():	# some cells have not been explored yet	
				print cell_list
				dfs_stack=[]
				first_unvisited_cell=None
				#find an unvisited cell
				for cell in cell_list:
					if not cell_visited[cell]:
						first_unvisited_cell=cell
						cell_visited[cell]=True
						break
				
				dfs_stack.append(first_unvisited_cell)
				#a list that maintains "virtual measurement" that can cause loop
				#the value means: if handoff happens, its rss MUST be greater than this value
				#instead of real minimin meas (-140dBm), we apply relative meas here
				virtual_rss=["x"]	#the first cell's rss is represted as a symbol

				while dfs_stack:
					src_cell = dfs_stack.pop()
					src_rss = virtual_rss.pop()
					dst_cell = None
					dst_rss = None

					for cell in cell_config_visited[src_cell]:
						if not cell_config_visited[src_cell][cell]:
							dst_cell = cell
							cell_config_visited[src_cell][dst_cell]=True
							cell_visited[dst_cell]=True
							break

					if dst_cell == None: #src's all neighbors have been visited
						continue

					src_freq=cell_config[src_cell].status.freq
					dst_freq=cell_config[dst_cell].status.freq

					dst_config=cell_config[src_cell].get_cell_reselection_config(dst_cell,dst_freq)

					if src_freq==dst_freq:	#intra-freq reselection
						dst_rss=src_rss+"+"+str(dst_config.offset)
						dfs_stack.append(src_cell)
						dfs_stack.append(dst_cell)
						virtual_rss.append(src_rss)
						virtual_rss.append(dst_rss)
					else:	#inter-freq/RAT reselection
						
						src_pref=cell_config[src_cell].sib.serv_config.priority
						dst_pref=dst_config.freq
		
						#Compare the preference
						if src_pref<dst_pref:
							dfs_stack.append(src_cell)
							dfs_stack.append(dst_cell)
							virtual_rss.append(src_rss)
							virtual_rss.append(str(dst_config.threshx_high))
						elif src_pref>dst_pref:
							threshserv=cell_config[src_cell].sib.serv_config.threshserv_low
							if src_rss >= threshserv:	#no loop, pass the dst_cell
								dfs_stack.append(src_cell)
								virtual_rss.append(src_rss)
							else:
								dfs_stack.append(src_cell)
								dfs_stack.append(dst_cell)
								virtual_rss.append(src_rss)
								virtual_rss.append(str(dst_config.threshx_high))
						else: #src_pref==dst_pref
							dst_rss=src_rss+"+"+str(dst_config.offset)
							dfs_stack.append(src_cell)
							dfs_stack.append(dst_cell)
							virtual_rss.append(src_rss)
							virtual_rss.append(dst_rss)

					#Loop report
					#TODO: optimize the code. Avoid list traverse 
					if dfs_stack.count(dst_cell)>1:
						#The first cell must be the 
						#First test if the duplicate cell's rss can satisfy the transition

						dfs_stack.pop()
						res_rss=virtual_rss.pop()
						print "res_rss:",res_rss
						if res_rss[0]=="x":
							res_rss=res_rss[2:] #equal-priority reselection loop
							if eval(res_rss)>=0:
								continue
						loop_report="Persistent loop: "
						for cell in dfs_stack:
							loop_report=loop_report+str(cell)+"->"
						loop_report=loop_report+str(dst_cell)
						print loop_report



