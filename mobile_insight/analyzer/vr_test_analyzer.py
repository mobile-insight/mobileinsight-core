#!/usr/bin/python
# Filename: wcdma_rrc_analyzer.py
"""
A WCDMA (3G) RRC analyzer.

Author: Yuanjie Li, Zhehui Zhang
"""

from analyzer import *
from state_machine import *
from protocol_analyzer import *
import timeit

__all__=["VrTestAnalyzer"]

class VrTestAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)
        self.add_source_callback(self.__msg_callback)

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages
        """
        Analyzer.set_source(self, source)

        source.enable_log("LTE_PHY_PUCCH_Tx_Report")
        source.enable_log("LTE_MAC_UL_Transport_Block")
        source.enable_log("LTE_MAC_UL_Buffer_Status_Internal")
        source.enable_log("LTE_PHY_PDSCH_Stat_Indication")
        source.enable_log("LTE_RLC_DL_AM_All_PDU")
        source.enable_log("LTE_RLC_UL_AM_All_PDU")

    def __msg_callback(self, msg):

        if msg.type_id == "LTE_PHY_PUCCH_Tx_Report":
            print str(msg.data.decode())
        elif msg.type_id == "LTE_MAC_UL_Transport_Block":
            print str(msg.data.decode())
        elif msg.type_id == "TE_MAC_UL_Buffer_Status_Internal":
            print str(msg.data.decode())
        elif msg.type_id == "LTE_PHY_PDSCH_Stat_Indication":
            print str(msg.data.decode())
        elif msg.type_id == "LTE_RLC_DL_AM_All_PDU":
            print str(msg.data.decode())
        elif msg.type_id == "LTE_RLC_UL_AM_All_PDU":
            print str(msg.data.decode())

