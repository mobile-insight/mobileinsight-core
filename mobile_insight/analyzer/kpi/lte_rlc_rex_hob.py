#!/usr/bin/python
# Filename: lte_rlc_retx_hob.py

"""
A file that count the number of pdcp packets blocked by rlc retransmission
(for downlink)

Author: Qianru Li
"""

from ..analyzer import *
from .kpi_analyzer import KpiAnalyzer

__all__=["LteRlcRetxHobAnalyzer"]

class LteRlcRetxHobAnalyzer(KpiAnalyzer):

	def __init__(self):

		KpiAnalyzer.__init__(self)

		self.add_source_callback(self.__rlc_hob_callback)


	def set_source(self, source):
		"""
		Set the source, enable RLC message and PDCP message
		"""
		KpiAnalyzer.set_source(self, source)
		source.enable_log("LTE_RLC_UL_AM_All_PDU")
		source.enable_log("LTE_RLC_DL_AM_All_PDU")
		source.enable_log("LTE_PDCP_DL_Cipher_Data_PDU")


	def __rlc_hob_callback(self, msg):
		if msg.type_id == "LTE_RLC_UL_AM_All_PDU":
			self.__rlc_ul_pdu_callback(msg)

		elif msg.type_id == "LTE_RLC_DL_AM_All_PDU":
			self.__rlc_dl_pdu_callback(msg)

		elif msg.type_id == "LTE_PDCP_DL_Cipher_Data_PDU":
			self.__pdcp_dl_pdu_callback(msg)

	def __rlc_ul_pdu_callback(self, msg):
		log_item = msg.data.decode()

	def __rlc_dl_pdu_callback(self, msg):
		log_item = msg.data.decode()

	def __pdcp_dl_pdu_callback(self, msg):
		log_item = msg.data.decode()
