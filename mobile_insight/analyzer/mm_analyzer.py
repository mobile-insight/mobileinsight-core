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
        self.__umts_rau = []
        self.__lte_normal_service = []
        self.__lte_plmn_search = []
        self.__lte_attach = []
        self.__lte_tau = []


    def set_source(self,source):
        """
        Set the trace source. Enable the WCDMA RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        Analyzer.set_source(self,source)

        source.enable_log("CDMA_Paging_Channel_Message")

        source.enable_log("1xEV_Signaling_Control_Channel_Broadcast")

        source.enable_log("UMTS_NAS_GMM_State")
        source.enable_log("UMTS_NAS_MM_State")
        source.enable_log("UMTS_NAS_OTA")
        source.enable_log("WCDMA_Signaling_Messages")

        source.enable_log("LTE_RRC_OTA_Packet")
        # source.enable_log("LTE_RRC_MIB_Message_Log_Packet")
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

    def get_umts_rau_log(self):
        """
        Return the RAU (Routing Area Update) time span of WCDMA network.
        """
        return self.__umts_rau

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

    def get_lte_attach_log(self):
        """
        Return the attach time span of LTE network.
        """
        return self.__lte_attach

    def get_lte_tau_log(self):
        """
        Return the TAU (Tracking Area Upate) time span of LTE network.
        """
        return self.__lte_tau


    def __filter(self, event):
        log_item = event.data.decode()
        decoded_event = Event(event.timestamp, event.type_id, log_item)
        
        if event.type_id == "CDMA_Paging_Channel_Message":
            self.__callback_cdma_paging_chann(decoded_event)
        elif event.type_id == "1xEV_Signaling_Control_Channel_Broadcast":
            self.__callback_1xev_broadcast_chann(decoded_event)
        elif event.type_id == "UMTS_NAS_MM_State":
            # print log_item["timestamp"], log_item["MM State"], log_item["MM Substate"]
            # Ignore
            pass
        elif event.type_id == "UMTS_NAS_GMM_State":
            self.__callback_umts_nas_gmm(decoded_event)
        elif event.type_id == "UMTS_NAS_OTA":
            self.__callback_umts_nas(decoded_event)
        elif event.type_id == "LTE_NAS_EMM_State":
            self.__callback_lte_nas_emm(decoded_event)
        elif event.type_id.startswith("LTE_NAS_ESM_Plain_OTA_") or event.type_id.startswith("LTE_NAS_EMM_Plain_OTA_"):
            self.__callback_lte_nas(decoded_event)
        elif event.type_id == "LTE_RRC_OTA_Packet":
            self.__callback_lte_rrc_ota(decoded_event)
        elif event.type_id == "LTE_RRC_Serv_Cell_Info_Log_Packet":
            self.__callback_lte_rrc_serv_cell_info(decoded_event)


    def __callback_cdma_paging_chann(self, event):
        log_item = event.data

        s = "CDMA"
        if in_span(self.__lte_plmn_search):
            l = self.__lte_plmn_search[-1].search_log
            if in_span(l) and l[-1].cell_id != s:
                end_span(l, log_item)
                start_span(l, log_item, cell_id=s)
            elif not in_span(l):
                start_span(l, log_item, cell_id=s)


    def __callback_1xev_broadcast_chann(self, event):
        log_item = event.data

        s = "1xEV/B%(Band)d-%(HSTR)d" % log_item
        if in_span(self.__lte_plmn_search):
            l = self.__lte_plmn_search[-1].search_log
            if in_span(l) and l[-1].cell_id != s:
                end_span(l, log_item)
                start_span(l, log_item, cell_id=s)
            elif not in_span(l):
                start_span(l, log_item, cell_id=s)


    def __callback_umts_nas_gmm(self, event):
        log_item = event.data
        # print log_item["timestamp"], " " * 30, log_item["GMM State"], log_item["GMM Substate"]

        # Normal service span
        if log_item["GMM State"] == "GMM_REGISTERED" and log_item["GMM Substate"] == "GMM_NORMAL_SERVICE":
            start_span(self.__umts_normal_service, log_item)
        elif {log_item["GMM State"], log_item["GMM Substate"]} & {"Unknown", "Undefined"}:
            pass
        else:
            end_span(self.__umts_normal_service, log_item)


    def __callback_umts_nas(self, event):
        log_item = event.data
        log_xml = ET.fromstring(log_item['Msg'])

        nas_type = ""
        for val in log_xml.iter("field"):
            if val.get("name") in {"gsm_a.dtap.msg_mm_type", "gsm_a.dtap.msg_gmm_type", "gsm_a.dtap.msg_sm_type"}:
                s = val.get("showname")
                nas_type = re.findall(r": (.*) \(0x[\da-fA-F]+\)$", s)[0]
                break
        # print nas_type

        # WCDMA Routing Area Update
        if nas_type in {"Routing Area Update Request"}:
            start_span(self.__umts_rau, log_item, request=nas_type, response=None)
        elif nas_type in {"Routing Area Update Complete", "Routing Area Update Reject"}:
            if in_span(self.__umts_rau):
                end_span(self.__umts_rau, log_item)
                self.__umts_rau[-1].response = nas_type


    def __callback_lte_nas_emm(self, event):
        log_item = event.data
        # print log_item["timestamp"], " " * 60, log_item["EMM State"], log_item["EMM Substate"]

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
        for val in log_xml.iter("field"):
            if val.get("name") in {"nas_eps.nas_msg_emm_type", "nas_eps.nas_msg_esm_type"}:
                s = val.get("showname")
                nas_type = re.findall(r": (.*) \(0x[\da-fA-F]+\)", s)[0]
                break
        # print nas_type        

        # LTE Attach
        if nas_type in {"Attach request"}:
            start_span(self.__lte_attach, log_item, request=nas_type, response=None)
        elif nas_type in {"Attach complete", "Attach reject"}:
            if in_span(self.__lte_attach):
                end_span(self.__lte_attach, log_item)
                self.__lte_attach[-1].response = nas_type
        
        # LTE Tracking Area Update
        if nas_type in {"Tracking area update request"}:
            start_span(self.__lte_tau, log_item, request=nas_type, response=None)
            # print log_item["Msg"]
            # raise RuntimeError()
        elif nas_type in {"Tracking area update complete", "Tracking area update reject"}:
            if in_span(self.__lte_tau):
                end_span(self.__lte_tau, log_item)
                self.__lte_tau[-1].response = nas_type


    def __callback_lte_rrc_ota(self, event):
        log_item = event.data
        log_xml = ET.fromstring(log_item['Msg'])

        is_sib1 = False
        info = {"plmn": None, "tac": None, "cell_id": None}
        if log_item["PDU Number"] == 2: # BCCH_DL_SCH
            for val in log_xml.iter("field"):
                if val.get("name") == "lte-rrc.systemInformationBlockType1_element":
                    is_sib1 = True
                elif val.get("name") == "lte-rrc.plmn_Identity_element":
                    mcc_mnc = ""
                    for digit in val.iter("field"):
                        if digit.get("name") == "lte-rrc.MCC_MNC_Digit":
                            mcc_mnc += digit.get("show")
                    info["plmn"] = mcc_mnc[0:3] + "-" + mcc_mnc[3:]
                elif val.get("name") == "lte-rrc.trackingAreaCode":
                    info["tac"] = int(val.get("value"), base=16)
                elif val.get("name") == "lte-rrc.cellIdentity":
                    info["cell_id"] = int(val.get("value"), base=16) / 16

        if is_sib1 and in_span(self.__lte_plmn_search):
            s = "LTE/%(plmn)s-%(tac)d-%(cell_id)d" % info
            l = self.__lte_plmn_search[-1].search_log
            if in_span(l) and l[-1].cell_id != s:
                end_span(l, log_item)
                start_span(l, log_item, cell_id=s)
            elif not in_span(l):
                start_span(l, log_item, cell_id=s)


    def __callback_lte_rrc_serv_cell_info(self, event):
        log_item = event.data

        if log_item["MNC Digit"] == 3:
            s = "LTE/%(MCC)03d-%(MNC)03d-%(TAC)d-%(Cell Identity)d" % log_item
        elif log_item["MNC Digit"] == 2:
            s = "LTE/%(MCC)03d-%(MNC)02d-%(TAC)d-%(Cell Identity)d" % log_item
        if in_span(self.__lte_plmn_search):
            l = self.__lte_plmn_search[-1].search_log
            if in_span(l) and l[-1].cell_id != s:
                end_span(l, log_item)
                start_span(l, log_item, cell_id=s)
            elif not in_span(l):
                start_span(l, log_item, cell_id=s)
