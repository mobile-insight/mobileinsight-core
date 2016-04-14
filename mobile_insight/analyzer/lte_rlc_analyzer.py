#!/usr/bin/python
# Filename: lte_rlc_analyzer.py
"""
A 4G RLC analyzer to get link layer information

Author: Haotian Deng
"""


from mobile_insight.analyzer.analyzer import *
from xml.dom import minidom

__all__=["LteRlcAnalyzer"]

class LteRlcAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.init_timestamp=None


    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        #Phy-layer logs
        source.enable_log("LTE_RLC_UL_Config_Log_Packet")
        source.enable_log("LTE_RLC_DL_Config_Log_Packet")
        # source.enable_log("LTE_RLC_UL_AM_All_PDU")
        # source.enable_log("LTE_RLC_DL_AM_All_PDU")

    def __msg_callback(self,msg):

        if msg.type_id == "LTE_RLC_UL_Config_Log_Packet" or \
                msg.type_id == "LTE_RLC_DL_Config_Log_Packet" or \
                msg.type_id == "LTE_RLC_UL_AM_All_PDU" or \
                msg.type_id == "LTE_RLC_DL_AM_All_PDU":
            s = msg.data.decode_xml().replace("\n", "")
            print minidom.parseString(s).toprettyxml(" ")
            log_item = msg.data.decode()

            print log_item

            # if not self.init_timestamp:
            #     self.init_timestamp = log_item['timestamp']
            #     self.log_info("0 "
            #     + str(log_item["MCS 0"])+" "
            #     + str(log_item["MCS 1"])+" "
            #     + str(log_item["TBS 0"])+" "
            #     + str(log_item["TBS 1"]))
            # else:
            #     self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+" "
            #     + str(log_item["MCS 0"])+" "
            #     + str(log_item["MCS 1"])+" "
            #     + str(log_item["TBS 0"])+" "
            #     + str(log_item["TBS 1"]))

