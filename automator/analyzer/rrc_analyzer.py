#! /usr/bin/env python
"""
rrc_analyzer.py

A RRC analyzer that integrates LTE and WCDMA RRC

Author: Yuanjie Li
"""

import xml.etree.ElementTree as ET
from analyzer import *
from wcdma_rrc_analyzer import WcdmaRrcAnalyzer
from lte_rrc_analyzer import LteRrcAnalyzer


class RrcAnalyzer(Analyzer):

	def __init__(self):

		Analyzer.__init__(self)

		#include analyzers
		self.lte_rrc_analyzer=LteRrcAnalyzer()
		self.wcdma_rrc_analyzer=WcdmaRrcAnalyzer()

		self.include_analyzer(self.lte_rrc_analyzer,[])
		self.include_analyzer(self.wcdma_rrc_analyzer,[])

		self.cur_RAT = None	#current RAT

		#init packet filters
		self.add_source_callback(self.rrc_filter)

	def rrc_filter(self,msg):

		"""
			Decide the current RAT
		"""
		if msg.type_id.find("LTE")!=-1:	#LTE RRC msg received, so it's LTE
			self.cur_RAT = "LTE"
		elif msg.type_id.find("WCDMA")!=-1:	#WCDMA RRC msg received, so it's WCDMA
			self.cur_RAT = "WCDMA"

		cur_cell_config=self.get_cur_cell_config()
		if cur_cell_config != None:
			cur_cell_config.status.dump()

	def get_cell_list(self):
		lte_cell_list=self.lte_rrc_analyzer.get_cell_list()
		wcdma_cell_list=self.wcdma_rrc_analyzer.get_cell_list()
		return lte_cell_list+wcdma_cell_list

	def get_cell_config(self,cell_id):

		"""
			get RRC configuration of a cell
			Assumption: the cell ID is globally unique in LTE AND WCDMA
		"""
		res=self.lte_rrc_analyzer.get_cell_config(cell_id)
		if res != None:
			return res
		else:
			return self.wcdma_rrc_analyzer.get_cell_config(cell_id)

	def get_cur_cell(self):
		"""
			Return a cell's configuration
			TODO: if no current cell (inactive), return None
		"""
		if self.cur_RAT=="LTE":
			self.lte_rrc_analyzer.get_cur_cell().dump()
			return self.lte_rrc_analyzer.get_cur_cell()
		elif self.cur_RAT=="WCDMA":
			self.wcdma_rrc_analyzer.get_cur_cell().dump()
			return self.wcdma_rrc_analyzer.get_cur_cell()
		else:
			return None

	def get_cur_cell_config(self):
		"""
			Return current cell's configuration
		"""
		if self.cur_RAT=="LTE":
			return self.lte_rrc_analyzer.get_cur_cell_config()
		elif self.cur_RAT=="WCDMA":
			return self.wcdma_rrc_analyzer.get_cur_cell_config()
		else:
			return None





