#!/usr/bin/python
# Filename: lte_pdcp_analyzer.py
"""
A 4G PDCP analyzer to get link layer information

Author: Haotian Deng
"""


from mobile_insight.analyzer.analyzer import *
from xml.dom import minidom

__all__ = ["LtePdcpAnalyzer"]


class LtePdcpAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("LTE_PDCP_DL_Config")
        source.enable_log("LTE_PDCP_UL_Config")
        source.enable_log("LTE_PDCP_UL_Data_PDU")
        source.enable_log("LTE_PDCP_DL_Ctrl_PDU")
        source.enable_log("LTE_PDCP_UL_Ctrl_PDU")
        source.enable_log("LTE_PDCP_DL_Stats")
        source.enable_log("LTE_PDCP_UL_Stats")
        source.enable_log("LTE_PDCP_DL_SRB_Integrity_Data_PDU")
        source.enable_log("LTE_PDCP_UL_SRB_Integrity_Data_PDU")

    def __msg_callback(self, msg):
        s = msg.data.decode_xml().replace("\n", "")
        print((minidom.parseString(s).toprettyxml(" ")))
