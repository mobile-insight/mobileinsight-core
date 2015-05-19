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

__all__=["RrcAnalyzer"]

class RrcAnalyzer(Analyzer):

	def __init__(self):

		Analyzer.__init__(self)

		#include analyzers
		self.__lte_rrc_analyzer=LteRrcAnalyzer()
		self.__wcdma_rrc_analyzer=WcdmaRrcAnalyzer()

		self.include_analyzer(self.__lte_rrc_analyzer,[])
		self.include_analyzer(self.__wcdma_rrc_analyzer,[])

		self.__cur_RAT = None	#current RAT

		#init packet filters
		self.add_source_callback(self.__rrc_filter)

	def __rrc_filter(self,msg):

		"""
			Decide the current RAT
		"""
		if msg.type_id.find("LTE")!=-1:	#LTE RRC msg received, so it's LTE
			self.__cur_RAT = "LTE"
		elif msg.type_id.find("WCDMA")!=-1:	#WCDMA RRC msg received, so it's WCDMA
			self.__cur_RAT = "WCDMA"

		cur_cell_config=self.get_cur_cell_config()
		if cur_cell_config != None:
			cur_cell_config.status.dump()

	def get_cell_list(self):
		lte_cell_list=self.__lte_rrc_analyzer.get_cell_list()
		wcdma_cell_list=self.__wcdma_rrc_analyzer.get_cell_list()
		return lte_cell_list+wcdma_cell_list

	def get_cell_config(self,cell_id):

		"""
			get RRC configuration of a cell
			Assumption: the cell ID is globally unique in LTE AND WCDMA
		"""
		res=self.__lte_rrc_analyzer.get_cell_config(cell_id)
		if res != None:
			return res
		else:
			return self.__wcdma_rrc_analyzer.get_cell_config(cell_id)

	def get_cur_cell(self):
		"""
			Return a cell's configuration
			TODO: if no current cell (inactive), return None
		"""
		if self.__cur_RAT=="LTE":
			self.__lte_rrc_analyzer.get_cur_cell().dump()
			return self.lte_rrc_analyzer.get_cur_cell()
		elif self.__cur_RAT=="WCDMA":
			self.__wcdma_rrc_analyzer.get_cur_cell().dump()
			return self.__wcdma_rrc_analyzer.get_cur_cell()
		else:
			return None

	def get_cur_cell_config(self):
		"""
			Return current cell's configuration
		"""
		if self.__cur_RAT=="LTE":
			return self.__lte_rrc_analyzer.get_cur_cell_config()
		elif self.__cur_RAT=="WCDMA":
			return self.__wcdma_rrc_analyzer.get_cur_cell_config()
		else:
			return None





