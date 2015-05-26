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
		

		#test
		# print cell_list
		for cell in cell_list:
			self.__rrc_analyzer.get_cell_config(cell).dump()

		if cell_list:

			# We implement the loop detection algorithm in Proposition 3,
			# because preferences are observed to be inconsistent

			while False in cell_visited.itervalues():	# some cells have not been explored yet	

				# In each round, we report loops with *unvisited_cell* involved			
				unvisited_cell=None
				#find an unvisited cell
				for cell in cell_list:
					if not cell_visited[cell]:
						unvisited_cell=cell
						break

				# each cell's configuration
				cell_config = {}	
				#boolean matrix: [i][j] indicates if ci->cj has been visited
				cell_neighbor_visited = {} 
				for cell in cell_list:
					if not cell_visited[cell]:	#visited cells would not be considered
						cell_config[cell]=self.__rrc_analyzer.get_cell_config(cell)
						neighbor_cells = self.__rrc_analyzer.get_cell_neighbor(cell)
						cell_neighbor_visited[cell]={}
						for item in neighbor_cells:
							if not cell_visited[item]:
								cell_neighbor_visited[cell][item]=False


				dfs_stack=[unvisited_cell]		
				# For ci->ci+1, a ci's rss lower bound that satisifes this handoff
				# virtual (normalized) measurements are used
				virtual_rss=[0]
				# For dfs_stack[0]->dfs_stack[1], indicates whether ci's rss matters	
				dont_care=False

				while dfs_stack:
					print "dfs_stack",dfs_stack
					src_cell = dfs_stack.pop()
					src_rss = virtual_rss.pop()
					dst_cell = None
					dst_rss = None

					#Find a next cell to handoff
					for cell in cell_neighbor_visited[src_cell]:
						#eliminate duplicate: visited cells are not visited
						if not cell_neighbor_visited[src_cell][cell]\
						and not cell_visited[cell]\
						and (not dfs_stack or cell not in dfs_stack[1:]):	
							dst_cell = cell
							cell_neighbor_visited[src_cell][dst_cell]=True
							break

					# print "dst_cell",dst_cell
					if dst_cell==None:
						#src_cell's all neighbors have been visited
						continue

					src_freq=cell_config[src_cell].status.freq
					dst_freq=cell_config[dst_cell].status.freq
					dst_config=cell_config[src_cell].get_cell_reselection_config(cell_config[dst_cell].status)

					src_pref=cell_config[src_cell].sib.serv_config.priority
					if dst_config!=None:
						#WCDMA only
						dst_pref=dst_config.priority
					else:
						dst_pref=None

					# print src_freq,src_pref,dst_freq,dst_pref

					if src_pref==None or dst_pref==None:	#happens in 3G
						#25.331: without pref, treat as equal pref
						src_pref = dst_pref = None 

					# a potential loop with dst_cell
					if dst_cell in dfs_stack: 
						# dst_cell==dfs_stack[0]
						# dfs_stack and dont_care must not be empty!!!

						print src_freq,dst_freq,src_pref,dst_pref,dont_care
						
						loop_happen = False
						if dont_care:
							# print "test1"
							#loop if src_cell->dst_cell happens under src_rss only

							#intra-freq: loop must happens
							intra_freq_loop = (src_freq==dst_freq)
							#inter-freq/RAT: loop happens in equal/high-pref reselection
							inter_freq_loop1 = (src_freq!=dst_freq and src_pref<=dst_pref)
							#inter-freq/RAT: low-pref reselection happens
							inter_freq_loop2 = (src_freq!=dst_freq and src_pref>dst_pref \
							and src_rss<cell_config[src_cell].sib.serv_config.threshserv_low)

							loop_happen = intra_freq_loop or inter_freq_loop1 \
								or inter_freq_loop2
						else:
							# print "test2"
							#loop if src_cell->dst_cell happens under src_rss and dst_rss
							dst_rss = virtual_rss[0]

							intra_freq_loop = (src_freq==dst_freq \
								and dst_rss>=src_rss+dst_config.offset)

							inter_freq_loop1 = (src_freq!=dst_freq and src_pref==dst_pref \
								and dst_rss>=src_rss+dst_config.offset) 

							inter_freq_loop2 = (src_freq!=dst_freq and src_pref<dst_pref \
								and dst_rss>=dst_config.threshx_high)

							inter_freq_loop3 = (src_freq!=dst_freq and src_pref>dst_pref \
								and src_rss<cell_config[src_cell].sib.serv_config.threshserv_low \
								and dst_rss>=dst_config.threshx_low)

							loop_happen = intra_freq_loop or inter_freq_loop1 \
								or inter_freq_loop2 or inter_freq_loop3

						if loop_happen:
							# print "test3"
							#report loop
							loop_report="\033[91m\033[1mPersistent loop: \033[0m\033[0m"
							for cell in dfs_stack:
								loop_report=loop_report+str(cell)+"->"
							loop_report=loop_report+str(src_cell)+"->"+str(dst_cell)
							print loop_report
							
						dfs_stack.append(src_cell)
						virtual_rss.append(src_rss)
						continue

					else:
						if src_freq==dst_freq:	#intra-freq reselection
							# print "test4"

							if dst_config.offset!=None:
								if not dfs_stack:
									dont_care = False
								dst_rss=src_rss+dst_config.offset
								dfs_stack.append(src_cell)
								dfs_stack.append(dst_cell)
								virtual_rss.append(src_rss)
								virtual_rss.append(dst_rss)
							else: #trace not ready
								dfs_stack.append(src_cell)
								virtual_rss.append(src_rss)
						else:
							if src_pref<dst_pref:
								# print "test5"
								if not dfs_stack:
									dont_care = True
								dfs_stack.append(src_cell)
								dfs_stack.append(dst_cell)
								virtual_rss.append(src_rss)
								virtual_rss.append(dst_config.threshx_high)
							elif src_pref>dst_pref:

								threshserv=cell_config[src_cell].sib.serv_config.threshserv_low
								if src_rss >= threshserv:	#no loop, pass the dst_cell
									# print "test6"
									dfs_stack.append(src_cell)
									virtual_rss.append(src_rss)
								else:
									# print "test7"
									if not dfs_stack:
										dont_care = False
									dfs_stack.append(src_cell)
									dfs_stack.append(dst_cell)
									virtual_rss.append(src_rss)
									virtual_rss.append(dst_config.threshx_low)
							else:	#src_pref==dst_pref
								# print "test8"
								if not dfs_stack:
									dont_care = False

								#test
								if dst_config.offset!=None:
									dst_rss=src_rss+dst_config.offset
									dfs_stack.append(src_cell)
									dfs_stack.append(dst_cell)
									virtual_rss.append(src_rss)
									virtual_rss.append(dst_rss)
								else:
									dfs_stack.append(src_cell)
									virtual_rss.append(src_rss)

				cell_visited[unvisited_cell]=True

