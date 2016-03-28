#!/usr/bin/python
# Filename: msg_statistics.py
"""
A simple analyzer to study the cellular message statistics and arrival interval time

Author: Yuanjie Li
"""


from mobile_insight.analyzer.analyzer import *

__all__=["MsgStatistics"]

class MsgStatistics(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.msg_type_statistics={} #type_id->msg_count

        self.msg_arrival_rate={} #type_id->list of arrival interval

        self.msg_lengh={} #type_id->list of message length

    def reset(self):
        self.msg_type_statistics={} #type_id->msg_count

        self.msg_arrival_rate={} #type_id->list of arrival interval

        self.msg_lengh={} #type_id->list of message length


    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)
        source.enable_log_all()

        # source.enable_log("WCDMA_RRC_OTA_Packet")
        # source.enable_log("LTE_RRC_OTA_Packet")
        # #Enable EMM/ESM logs
        # source.enable_log("UMTS_NAS_OTA_Packet")
        # source.enable_log("LTE_NAS_ESM_OTA_Incoming_Packet")
        # source.enable_log("LTE_NAS_ESM_OTA_Outgoing_Packet")
        # source.enable_log("LTE_NAS_EMM_OTA_Incoming_Packet")
        # source.enable_log("LTE_NAS_EMM_OTA_Outgoing_Packet")

        # #Phy-layer logs
        # source.enable_log("LTE_PHY_PDSCH_Packet")

        # source.enable_log("LTE_MAC_Configuration")
        # source.enable_log("LTE_MAC_UL_Transport_Block")
        # source.enable_log("LTE_MAC_DL_Transport_Block")

    def __msg_callback(self,msg):

    	log_item = msg.data.decode()
        
        if msg.type_id not in self.msg_type_statistics:
            self.msg_type_statistics[msg.type_id] = 1
        else:
            self.msg_type_statistics[msg.type_id] = self.msg_type_statistics[msg.type_id] + 1

        if msg.type_id not in self.msg_arrival_rate:
            self.msg_arrival_rate[msg.type_id]=[log_item["timestamp"]]
        else:
        	self.msg_arrival_rate[msg.type_id].append(log_item["timestamp"])

        if msg.type_id not in self.msg_lengh:

            if log_item.has_key("log_msg_len"):
                self.msg_lengh[msg.type_id]=[log_item["log_msg_len"]]
            elif log_item.has_key("Msg Length"):
                self.msg_lengh[msg.type_id]=[log_item["Msg Length"]]
            elif log_item.has_key("Message Length"):
                self.msg_lengh[msg.type_id]=[log_item["Message Length"]] 
            #TODO: For EMM/ESM, why the metadata header does not have msg length?
        else:
            if log_item.has_key("log_msg_len"):
                self.msg_lengh[msg.type_id].append(log_item["log_msg_len"])
            elif log_item.has_key("Msg Length"):
                self.msg_lengh[msg.type_id].append(log_item["Msg Length"])
            elif log_item.has_key("Message Length"):
                self.msg_lengh[msg.type_id].append(log_item["Message Length"])
        	









