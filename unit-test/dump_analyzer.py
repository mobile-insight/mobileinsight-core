#!/usr/bin/python
# Filename: dump_analyzer.py
"""
Simply dump packcet content in XML format

Author: Haotian Deng
"""


from mobile_insight.analyzer.analyzer import *
from xml.dom import minidom
import datetime
import os

__all__=["DumpAnalyzer"]

class DumpAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)
        self.mi2log = ""

    def close(self):
        pass

    def set_mi2log(self, path):
        self.mi2log = path

    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        # source.enable_log("LTE_RRC_OTA_Packet")
        # source.enable_log("LTE_RRC_Serv_Cell_Info")
        # source.enable_log("LTE_RRC_MIB_Packet")
        # source.enable_log("LTE_RRC_MIB_Message_Log_Packet")
        # source.enable_log("LTE_NAS_ESM_State")
        # source.enable_log("LTE_NAS_ESM_OTA_Incoming_Packet")
        # source.enable_log("LTE_NAS_ESM_OTA_Outgoing_Packet")
        # source.enable_log("LTE_NAS_EMM_State")
        # source.enable_log("LTE_NAS_EMM_OTA_Incoming_Packet")
        # source.enable_log("LTE_NAS_EMM_OTA_Outgoing_Packet")

        # source.enable_log("LTE_PDCP_DL_Config")
        # source.enable_log("LTE_PDCP_UL_Config")
        # source.enable_log("LTE_PDCP_UL_Data_PDU")
        # source.enable_log("LTE_PDCP_DL_Ctrl_PDU")
        # source.enable_log("LTE_PDCP_UL_Ctrl_PDU")
        # source.enable_log("LTE_PDCP_DL_Stats")
        # source.enable_log("LTE_PDCP_UL_Stats")
        # source.enable_log("LTE_PDCP_DL_SRB_Integrity_Data_PDU")
        # source.enable_log("LTE_PDCP_UL_SRB_Integrity_Data_PDU")

        # source.enable_log("LTE_RLC_UL_Config_Log_Packet")
        # source.enable_log("LTE_RLC_DL_Config_Log_Packet")
        # source.enable_log("LTE_RLC_UL_AM_All_PDU")
        # source.enable_log("LTE_RLC_DL_AM_All_PDU")
        # source.enable_log("LTE_RLC_UL_Stats")
        # source.enable_log("LTE_RLC_DL_Stats")

        # source.enable_log("LTE_MAC_Configuration")
        # source.enable_log("LTE_MAC_UL_Transport_Block")
        # source.enable_log("LTE_MAC_DL_Transport_Block")
        # source.enable_log("LTE_MAC_UL_Buffer_Status_Internal")
        # source.enable_log("LTE_MAC_UL_Tx_Statistics")
        # source.enable_log("LTE_MAC_Rach_Trigger")
        # source.enable_log("LTE_MAC_Rach_Attempt")

        # source.enable_log("LTE_PHY_PDSCH_Packet")
        # source.enable_log("LTE_PHY_Serv_Cell_Measurement")
        # source.enable_log("LTE_PHY_Connected_Mode_Intra_Freq_Meas")
        # source.enable_log("LTE_PHY_Inter_RAT_Measurement")
        # source.enable_log("LTE_PHY_Inter_RAT_CDMA_Measurement")

        # source.enable_log("LTE_PUCCH_Power_Control")
        # source.enable_log("LTE_PUSCH_Power_Control")
        # source.enable_log("LTE_PDCCH_PHICH_Indication_Report")
        # source.enable_log("LTE_PDSCH_Stat_Indication")
        # source.enable_log("LTE_PHY_System_Scan_Results")
        # source.enable_log("LTE_PHY_BPLMN_Cell_Request")
        # source.enable_log("LTE_PHY_BPLMN_Cell_Confirm")
        # source.enable_log("LTE_PHY_Serving_Cell_COM_Loop")
        # source.enable_log("LTE_PHY_PDCCH_Decoding_Result")
        # source.enable_log("LTE_PHY_PDSCH_Decoding_Result")
        # source.enable_log("LTE_PHY_PUSCH_Tx_Report")
        # source.enable_log("LTE_PHY_RLM_Report")
        # source.enable_log("LTE_PHY_PUSCH_CSF")
        # source.enable_log("LTE_PHY_CDRX_Events_Info")
        source.enable_log("WCDMA_RRC_States")


        # source.enable_log("1xEV_Rx_Partial_MultiRLP_Packet")
        # source.enable_log("1xEV_Connected_State_Search_Info")
        # # source.enable_log("1xEV_Signaling_Control_Channel_Broadcast")
        # source.enable_log("1xEV_Connection_Attempt")
        # source.enable_log("1xEV_Connection_Release")

        # source.enable_log("WCDMA_RRC_OTA_Packet")
        # source.enable_log("WCDMA_RRC_Serv_Cell_Info")

        # source.enable_log("UMTS_NAS_OTA_Packet")
        # source.enable_log("UMTS_NAS_GMM_State")
        # source.enable_log("UMTS_NAS_MM_State")
        # source.enable_log("UMTS_NAS_MM_REG_State")

    def __msg_callback(self,msg):
        s = msg.data.decode_xml().replace("\n", "")
        print minidom.parseString(s).toprettyxml(" ")
        # if "Unsupported" in s or "unsupported" in s or "(MI)Unknown" in s:
        #     print "Unsupported message or unknown message field appears.\n"
        #     print minidom.parseString(s).toprettyxml(" ")
        #     print str(self.mi2log) + "\n"
