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
		# mark if a cell has been visited
		cell_visited = {x:False for x in cell_list} 
		

		#test
		# print cell_list
		for cell in cell_list:
			self.__rrc_analyzer.get_cell_config(cell).dump()

		# each cell's configuration
		cell_config = {}
		for cell in cell_list:
			cell_config[cell]=self.__rrc_analyzer.get_cell_config(cell)
		
		if cell_list:

			# We implement the loop detection algorithm in Proposition 3,
			# because preferences are observed to be inconsistent

			while False in cell_visited.itervalues():	
				# some cells have not been explored yet	

				# In each round, we report loops with *unvisited_cell* involved			
				unvisited_cell=None
				#find an unvisited cell
				for cell in cell_list:
					if not cell_visited[cell]:
						unvisited_cell=cell
						break

				neighbor_cells = self.__rrc_analyzer.get_cell_neighbor(unvisited_cell)
				#For each cell: 0 for unvisited, 1 for idle-only visited, 2 for idle-active visited
				neighbor_visited = {x: 0 for x in neighbor_cells}

				#visited cells are ruled out
				for item in neighbor_cells:
					if cell_visited.has_key(item) and cell_visited[item]:
						neighbor_visited[item]=2
				
				#stacks
				dfs_stack=[unvisited_cell]	
				neighbor_stack=[neighbor_visited]
				# For ci->ci+1, a ci's rss lower bound that satisifes this handoff
				# virtual (normalized) measurements are used
				virtual_rss=[0]
				# For dfs_stack[0]->dfs_stack[1], indicates whether ci's rss matters	
				dont_care=False

				while dfs_stack:
					# print "dfs_stack",dfs_stack
					self.logger.debug("dfs_stack:"+str(dfs_stack))
					src_cell = dfs_stack.pop()
					src_rss = virtual_rss.pop()
					src_neighbor = neighbor_stack.pop()
					dst_cell = None
					dst_rss = None

					#Find a next cell to handoff
					for cell in src_neighbor:
						# if src_neighbor[cell]<2 \
						if src_neighbor[cell]<1 \
						and (not dfs_stack or cell not in dfs_stack[1:]):
							dst_cell = cell
							break

					if dst_cell==None:
						#src_cell's all neighbors have been visited
						continue

					src_neighbor[dst_cell]+=1
					self.logger.debug("dst_cell:"+str(dst_cell)\
						+" state:"+str(src_neighbor[dst_cell]))

					src_freq=cell_config[src_cell].status.freq
					dst_freq=cell_config[dst_cell].status.freq
					dst_config=None
					if src_neighbor[dst_cell]==1:
						#idle-state handoff
						dst_config=cell_config[src_cell].get_cell_reselection_config(cell_config[dst_cell].status)
					else:
						#active-state handoff
						dst_config=cell_config[src_cell].get_meas_config(cell_config[dst_cell].status)

					if dst_config==None:
						continue

					src_pref=cell_config[src_cell].sib.serv_config.priority
					dst_pref=dst_pref=dst_config.priority

					if src_pref==None or dst_pref==None:	#happens in 3G
						#25.331: without pref, treat as equal pref
						src_pref = dst_pref = None

					# a potential loop with dst_cell
					if dst_cell in dfs_stack: 
						loop_happen = False
						if dont_care:
							#loop if src_cell->dst_cell happens under src_rss only

							#intra-freq: loop must happens
							intra_freq_loop = (src_freq==dst_freq)
							#inter-freq/RAT: loop happens in equal/high-pref reselection
							inter_freq_loop1 = (src_freq!=dst_freq and src_pref<=dst_pref)
							#inter-freq/RAT: low-pref reselection happens
							inter_freq_loop2 = (src_freq!=dst_freq and src_pref>dst_pref \
							and src_rss<dst_config.threshserv_low)

							loop_happen = intra_freq_loop \
										or inter_freq_loop1 \
										or inter_freq_loop2
						else:
							#loop if src_cell->dst_cell happens under src_rss and dst_rss
							dst_rss = virtual_rss[0]

							intra_freq_loop = (src_freq==dst_freq \
								and dst_rss>=src_rss+dst_config.offset)

							inter_freq_loop1 = (src_freq!=dst_freq and src_pref==dst_pref \
								and dst_rss>=src_rss+dst_config.offset) 

							inter_freq_loop2 = (src_freq!=dst_freq and src_pref<dst_pref \
								and dst_rss>=dst_config.threshx_high)

							inter_freq_loop3 = (src_freq!=dst_freq and src_pref>dst_pref \
								and src_rss<dst_config.threshserv_low \
								and dst_rss>=dst_config.threshx_low)

							loop_happen = intra_freq_loop \
										or inter_freq_loop1 \
										or inter_freq_loop2 \
										or inter_freq_loop3

						dfs_stack.append(src_cell)
						virtual_rss.append(src_rss)
						neighbor_stack.append(src_neighbor)

						if loop_happen:
							#report loop
							loop_report="\033[91m\033[1mPersistent loop: \033[0m\033[0m"
							loop_report+=str(dfs_stack[0])
							prev_item=dfs_stack[0]
							for item in range(1,len(dfs_stack)):
								cell=dfs_stack[item]
								if neighbor_stack[item-1][cell]==1:
									loop_report+="(idle)->"+str(cell)
								elif neighbor_stack[item-1][cell]==2:
									loop_report+="(active)->"+str(cell)

							if src_neighbor[dst_cell]==1:
								loop_report+="(idle)->"+str(dst_cell)
							elif src_neighbor[dst_cell]==2:
								loop_report+="(active)->"+str(dst_cell)

							self.logger.warning(loop_report)

						continue

					else:

						dst_neighbor_cells=self.__rrc_analyzer.get_cell_neighbor(dst_cell)
						dst_neighbor={x: 0 for x in dst_neighbor_cells}
						for item in dst_neighbor:
							if cell_visited.has_key(item) and cell_visited[item]:
								dst_neighbor[item]=2

						if src_freq==dst_freq:	#intra-freq handoff
							if dst_config.offset!=None:
								if not dfs_stack:
									dont_care = False
								dst_rss=src_rss+dst_config.offset
								dfs_stack.append(src_cell)
								dfs_stack.append(dst_cell)
								virtual_rss.append(src_rss)
								virtual_rss.append(dst_rss)
								neighbor_stack.append(src_neighbor)
								neighbor_stack.append(dst_neighbor)
							else: #trace not ready
								dfs_stack.append(src_cell)
								virtual_rss.append(src_rss)
								neighbor_stack.append(src_neighbor)
						else:
							if src_pref<dst_pref:
								if not dfs_stack:
									dont_care = True
								dfs_stack.append(src_cell)
								dfs_stack.append(dst_cell)
								virtual_rss.append(src_rss)
								virtual_rss.append(dst_config.threshx_high)
								neighbor_stack.append(src_neighbor)
								neighbor_stack.append(dst_neighbor)
							elif src_pref>dst_pref:
								if src_rss >= dst_config.threshserv_low:	#no loop, pass the dst_cell
									dfs_stack.append(src_cell)
									virtual_rss.append(src_rss)
									neighbor_stack.append(src_neighbor)
								else:
									if not dfs_stack:
										dont_care = False
									dfs_stack.append(src_cell)
									dfs_stack.append(dst_cell)
									# #IMPORTANT to set proper inital value
									src_rss = dst_config.threshserv_low
									virtual_rss.append(src_rss)
									virtual_rss.append(dst_config.threshx_low)
									neighbor_stack.append(src_neighbor)
									neighbor_stack.append(dst_neighbor)
							else:	#src_pref==dst_pref
								if not dfs_stack:
									dont_care = False
								#test
								if dst_config.offset!=None:
									dst_rss=src_rss+dst_config.offset
									dfs_stack.append(src_cell)
									dfs_stack.append(dst_cell)
									virtual_rss.append(src_rss)
									virtual_rss.append(dst_rss)
									neighbor_stack.append(src_neighbor)
									neighbor_stack.append(dst_neighbor)
								else:
									dfs_stack.append(src_cell)
									virtual_rss.append(src_rss)
									neighbor_stack.append(src_neighbor)

				cell_visited[unvisited_cell]=True