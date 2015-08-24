#!/usr/bin/python
# Filename: mm_analyzer.py
"""


Author: Jiayao Li
"""

from analyzer import *

import xml.etree.ElementTree as ET
import datetime
import re

__all__ = ["MmAnalyzer"]


class Span(object):
    def __init__(self, start, end, **additional_info):
        self.start = start
        self.end = end
        for k, v in additional_info.items():
            setattr(self, k, v)

    def __repr__(self):
        s = "<start=%s, end=%s" % (repr(self.start), repr(self.end))
        for k, v in vars(self).items():
            if k not in {"start", "end"}:
                s += ", %s=%s" % (k, repr(v))
        s += ">"
        return s


def in_span(service_log):
    return len(service_log) > 0 and service_log[-1].end is None

def start_span(service_log, log_item, **additional_info):
    if not in_span(service_log):
        service_log.append(Span(log_item["timestamp"], None, **additional_info))

def end_span(service_log, log_item):
    if in_span(service_log):
        service_log[-1].end = log_item["timestamp"]



class MmAnalyzer(Analyzer):
    """
    Analyze the MM state change of the phone.
    """
    
    def __init__(self):
        Analyzer.__init__(self)
        self.add_source_callback(self.__filter)

        self.__umts_normal_service = []
        self.__lte_normal_service = []
        self.__lte_plmn_search = []
        self.__lte_other = []


    def set_source(self,source):
        """
        Set the trace source. Enable the WCDMA RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        Analyzer.set_source(self,source)

        source.enable_log("UMTS_NAS_GMM_State")
        source.enable_log("UMTS_NAS_MM_State")
        source.enable_log("UMTS_NAS_OTA")
        source.enable_log("WCDMA_Signaling_Messages")

        source.enable_log("LTE_RRC_OTA_Packet")
        source.enable_log("LTE_RRC_Serv_Cell_Info_Log_Packet")
        source.enable_log("LTE_NAS_EMM_State")
        source.enable_log("LTE_NAS_ESM_Plain_OTA_Incoming_Message")
        source.enable_log("LTE_NAS_ESM_Plain_OTA_Outgoing_Message")
        source.enable_log("LTE_NAS_EMM_Plain_OTA_Incoming_Message")
        source.enable_log("LTE_NAS_EMM_Plain_OTA_Outgoing_Message")


    def get_umts_normal_service_log(self):
        """
        Return the normal service time span of WCDMA network.
        """
        return self.__umts_normal_service


    def get_lte_normal_service_log(self):
        """
        Return the normal service time span of LTE network.
        """
        return self.__lte_normal_service


    def get_lte_plmn_search_log(self):
        """
        Return the PLMN search time span of LTE network, as well as how long the 
        phone spends on searching each cell.
        """
        return self.__lte_plmn_search

    def get_lte_other_log(self):
        """
        Return the other time span of LTE network (attach, tracking area update, etc.).
        """
        return self.__lte_other


    def __filter(self, event):
        log_item = event.data.decode()

        decoded_event = Event(event.timestamp, event.type_id, log_item)
        
        if event.type_id == "UMTS_NAS_MM_State":
            # Ignore
            pass
        elif event.type_id == "UMTS_NAS_GMM_State":
            self.__callback_umts_nas_gmm(decoded_event)
        elif event.type_id == "UMTS_NAS_OTA":
            # Ignore
            pass
        elif event.type_id == "LTE_NAS_EMM_State":
            self.__callback_lte_nas_emm(decoded_event)
        elif event.type_id.startswith("LTE_NAS_ESM_Plain_OTA_") or event.type_id.startswith("LTE_NAS_EMM_Plain_OTA_"):
            self.__callback_lte_nas(decoded_event)
        elif event.type_id == "LTE_RRC_Serv_Cell_Info_Log_Packet":
            self.__callback_lte_rrc_serv_cell_info(decoded_event)


    def __callback_umts_nas_gmm(self, event):
        log_item = event.data
        print log_item["timestamp"], " " * 30, log_item["GMM State"], log_item["GMM Substate"]


    def __callback_lte_nas_emm(self, event):
        log_item = event.data
        print log_item["timestamp"], " " * 60, log_item["EMM State"], log_item["EMM Substate"]

        # Normal service span
        if log_item["EMM Substate"] == "EMM_REGISTERED_NORMAL_SERVICE":
            start_span(self.__lte_normal_service, log_item)
        elif log_item["EMM Substate"] in {"Unknown", "Undefined"}:
            pass
        else:
            end_span(self.__lte_normal_service, log_item)

        # PLMN service span
        if log_item["EMM Substate"] in {"EMM_DEREGISTERED_PLMN_SEARCH", "EMM_REGISTERED_PLMN_SEARCH"}:
            start_span(self.__lte_plmn_search, log_item, search_log=[])
        elif log_item["EMM Substate"] == "EMM_REGISTERED_NORMAL_SERVICE":
            if len(self.__lte_plmn_search) > 0:
                end_span(self.__lte_plmn_search[-1].search_log, log_item)
            end_span(self.__lte_plmn_search, log_item)


    def __callback_lte_nas(self, event):
        log_item = event.data
        log_xml = ET.fromstring(log_item['Msg'])

        nas_type = ""
        for val in log_xml.iter('field'):
            if val.get("name") in {"nas_eps.nas_msg_emm_type", "nas_eps.nas_msg_esm_type"}:
                s = val.get("showname")
                nas_type = re.findall(r": (.*) \(0x[0-9a-fA-F]+\)", s)[0]
                break
        print nas_type
        
        if nas_type in {"Attach request", "Tracking area update request"}:
            start_span(self.__lte_other, log_item, request=nas_type, response=None)
        elif nas_type in {"Attach complete", "Attach reject"}:
            if in_span(self.__lte_other) and self.__lte_other[-1].request == "Attach request":
                end_span(self.__lte_other, log_item)
                self.__lte_other[-1].response = nas_type
        elif nas_type in {"Tracking area update complete", "Tracking area update reject"}:
            if in_span(self.__lte_other) and self.__lte_other[-1].request == "Tracking area update request":
                end_span(self.__lte_other, log_item)
                self.__lte_other[-1].response = nas_type
        
        


    def __callback_lte_rrc_serv_cell_info(self, event):
        log_item = event.data

        if log_item["MNC Digit"] == 3:
            s = "%(MCC)03d-%(MNC)03d-%(TAC)d-%(Cell Identity)d" % log_item
        elif log_item["MNC Digit"] == 2:
            s = "%(MCC)03d-%(MNC)02d-%(TAC)d-%(Cell Identity)d" % log_item
        if in_span(self.__lte_plmn_search):
            end_span(self.__lte_plmn_search[-1].search_log, log_item)
            start_span(self.__lte_plmn_search[-1].search_log, log_item, cell_id=s)
