#!/usr/bin/python
# Filename: mm_analyzer.py
"""


Author: Jiayao Li
"""

from .analyzer import *

import xml.etree.ElementTree as ET
import datetime
import re

__all__ = ["MmAnalyzer"]


class Span(object):
    def __init__(self, start, end, **additional_info):
        self.start = start
        self.end = end
        for k, v in list(additional_info.items()):
            setattr(self, k, v)

    def __repr__(self):
        s = "<start=%s, end=%s" % (repr(self.start), repr(self.end))
        for k, v in list(vars(self).items()):
            if k not in {"start", "end"}:
                s += ", %s=%s" % (k, repr(v))
        s += ">"
        return s


def in_span(service_log):
    return len(service_log) > 0 and service_log[-1].end is None


def start_span(service_log, log_item, **additional_info):
    if not in_span(service_log):
        service_log.append(
            Span(
                log_item["timestamp"],
                None,
                **additional_info))


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
        self.__umts_plmn_search = []
        self.__umts_attach = []
        self.__umts_lu = []
        self.__umts_rau = []
        self.__lte_normal_service = []
        self.__lte_plmn_search = []
        self.__lte_attach = []
        self.__lte_tau = []
        self.__lte_tau_qos_info = []
        self.__lte_cell_resel_to_umts_config = []
        self.__lte_drx_config = []
        self.__lte_tdd_config = []

        self.__last_normal_service = ""
        self.__last_lte_rrc_freq = 0
        self.__last_valid_timestamp = None
        self.__last_wcdma_rrc_mib_info = None
        self.__n_lte_rrc_reconfig = 0

    def set_source(self, source):
        """
        Set the trace source. Enable the WCDMA RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        Analyzer.set_source(self, source)

        source.enable_log_all()

    def get_umts_normal_service_log(self):
        """
        Return the normal service time span of WCDMA network.
        """
        return self.__umts_normal_service

    def get_umts_plmn_search_log(self):
        """
        Return the PLMN search time span of WCDMA network.
        """
        return self.__umts_plmn_search

    def get_umts_attach_log(self):
        """
        Return the attach time span of WCDMA network.
        """
        return self.__umts_attach

    def get_umts_lu_log(self):
        """
        Return the Location Update time span of WCDMA network.
        """
        return self.__umts_lu

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

    def get_lte_tau_qos_info(self):
        return self.__lte_tau_qos_info

    def get_lte_cell_resel_to_umts_config(self):
        return self.__lte_cell_resel_to_umts_config

    def get_lte_drx_config(self):
        return self.__lte_drx_config

    def get_lte_tdd_config(self):
        return self.__lte_tdd_config

    def get_n_lte_rrc_reconfig(self):
        return self.__n_lte_rrc_reconfig

    def __filter(self, event):
        log_item = event.data.decode()
        decoded_event = Event(event.timestamp, event.type_id, log_item)

        # Deal with out-of-order timestamps
        this_ts = log_item["timestamp"]
        if this_ts.year != 1980:    # Ignore undefined timestamp
            if self.__last_valid_timestamp:
                sec = (this_ts - self.__last_valid_timestamp).total_seconds()
                if sec >= 1200 or sec <= -120:
                    self.__pause(self.__last_valid_timestamp)
            self.__last_valid_timestamp = this_ts

        if event.type_id == "CDMA_Paging_Channel_Message":
            self.__callback_cdma_paging_chann(decoded_event)
        elif event.type_id == "1xEV_Signaling_Control_Channel_Broadcast":
            self.__callback_1xev_broadcast_chann(decoded_event)
        elif event.type_id == "UMTS_NAS_MM_State":
            # Ignore
            pass
        elif event.type_id == "UMTS_NAS_GMM_State":
            self.__callback_umts_nas_gmm(decoded_event)
        elif event.type_id == "UMTS_NAS_OTA_Packet":
            self.__callback_umts_nas(decoded_event)
        elif event.type_id == "WCDMA_RRC_Serv_Cell_Info":
            self.__callback_wcdma_cell_id(decoded_event)
        elif event.type_id == "WCDMA_RRC_OTA_Packet":
            if "Msg" in log_item:
                self.__callback_wcdma_rrc_ota(decoded_event)
        elif event.type_id == "LTE_NAS_EMM_State":
            self.__callback_lte_nas_emm(decoded_event)
        elif event.type_id.startswith("LTE_NAS_ESM_Plain_OTA_") or event.type_id.startswith("LTE_NAS_EMM_Plain_OTA_"):
            self.__callback_lte_nas(decoded_event)
        elif event.type_id == "LTE_RRC_OTA_Packet":
            self.__callback_lte_rrc_ota(decoded_event)
        elif event.type_id == "LTE_RRC_Serv_Cell_Info":
            self.__callback_lte_rrc_serv_cell_info(decoded_event)

    def __pause(self, last_valid_timestamp):
        log_item = {"timestamp": last_valid_timestamp}

        self.__last_normal_service = ""
        end_span(self.__umts_normal_service, log_item)
        end_span(self.__lte_normal_service, log_item)
        self.__end_plmn_search(log_item)

    def __start_plmn_search(self, network, last_normal_service, log_item):
        if network == "LTE":
            start_span(self.__lte_plmn_search, log_item,
                       search_log=[],
                       from_where=last_normal_service,
                       network=network)
        elif network == "UMTS":
            start_span(self.__umts_plmn_search, log_item,
                       search_log=[],
                       from_where=last_normal_service,
                       network=network)
        else:
            raise RuntimeError("wtf")

    def __add_plmn_search_cell(self, cell_id, log_item):
        if in_span(self.__umts_plmn_search):
            l = self.__umts_plmn_search[-1].search_log
            if in_span(l) and l[-1].cell_id != cell_id:
                end_span(l, log_item)
                start_span(l, log_item, cell_id=cell_id)
            elif not in_span(l):
                start_span(l, log_item, cell_id=cell_id)
        if in_span(self.__lte_plmn_search):
            l = self.__lte_plmn_search[-1].search_log
            if in_span(l) and l[-1].cell_id != cell_id:
                end_span(l, log_item)
                start_span(l, log_item, cell_id=cell_id)
            elif not in_span(l):
                start_span(l, log_item, cell_id=cell_id)

    def __end_plmn_search(self, log_item):
        # end potential WCDMA PLMN search
        if in_span(self.__umts_plmn_search):
            end_span(self.__umts_plmn_search[-1].search_log, log_item)
            end_span(self.__umts_plmn_search, log_item)
        # end potential LTE PLMN search
        if in_span(self.__lte_plmn_search):
            end_span(self.__lte_plmn_search[-1].search_log, log_item)
            end_span(self.__lte_plmn_search, log_item)

    def __callback_cdma_paging_chann(self, event):
        log_item = event.data

        s = "CDMA"
        self.__add_plmn_search_cell(s, log_item)

    def __callback_1xev_broadcast_chann(self, event):
        log_item = event.data

        s = "1xEV/B%(Band)d-%(HSTR)d" % log_item
        self.__add_plmn_search_cell(s, log_item)

    def __callback_umts_nas_gmm(self, event):
        log_item = event.data

        last_normal_service = self.__last_normal_service

        # Normal service span
        if log_item["GMM State"] == "GMM_REGISTERED" and log_item["GMM Substate"] == "GMM_NORMAL_SERVICE":
            start_span(self.__umts_normal_service, log_item)
            # This msg does not provide detailed information about the current
            # serving provider, so if we have extracted more detailed information
            # from other msgs, we do not update __last_normal_service.
            if not self.__last_normal_service:
                self.__last_normal_service = "WCDMA/Unknown"
        elif {log_item["GMM State"], log_item["GMM Substate"]} & {"Unknown", "Undefined"}:
            pass
        else:
            end_span(self.__umts_normal_service, log_item)

        # PLMN service span
        if log_item["GMM Substate"] == "GMM_PLMN_SEARCH":
            self.__start_plmn_search("UMTS", last_normal_service, log_item)
        elif log_item["GMM State"] == "GMM_REGISTERED" and log_item["GMM Substate"] == "GMM_NORMAL_SERVICE":
            self.__end_plmn_search(log_item)

    def __callback_wcdma_rrc_ota(self, event):
        log_item = event.data
        # log_xml = ET.fromstring(log_item["Msg"])
        log_xml = ET.XML(log_item["Msg"])

        mib = None
        sib3 = None
        for val in log_xml.iter("field"):
            if val.get("name") == "rrc.MasterInformationBlock_element":
                mib = val
            if val.get("name") == "rrc.SysInfoType3_element":
                sib3 = val

        if mib is not None:
            self.__callback_wcdma_rrc_ota_mib(event, mib)

        if sib3 is not None:
            self.__callback_wcdma_rrc_ota_sib3(event, sib3)

    def __callback_wcdma_rrc_ota_mib(self, event, mib):
        log_item = event.data

        info = {"mcc": None, "mnc": None}
        for val in mib.iter("field"):
            if val.get("name") == "rrc.mcc":
                mcc = ""
                for digit in val.iter("field"):
                    if digit.get("name") == "rrc.Digit":
                        mcc += digit.get("show")
                info["mcc"] = mcc
            elif val.get("name") == "rrc.mnc":
                mnc = ""
                for digit in val.iter("field"):
                    if digit.get("name") == "rrc.Digit":
                        mnc += digit.get("show")
                info["mnc"] = mnc

        self.__last_wcdma_rrc_mib_info = info

    def __callback_wcdma_rrc_ota_sib3(self, event, sib3):
        log_item = event.data

        if not self.__last_wcdma_rrc_mib_info:
            return

        cell_id = ""
        for val in sib3.iter("field"):
            if val.get("name") == "rrc.cellIdentity":
                c = int(val.get("value"), base=16) / 16
                cell_id = "WCDMA/%(mcc)s-%(mnc)s" % self.__last_wcdma_rrc_mib_info
                cell_id += "-%d" % c
                break

        if cell_id:
            self.__add_plmn_search_cell(cell_id, log_item)

    def __callback_umts_nas(self, event):
        log_item = event.data
        # log_xml = ET.fromstring(log_item["Msg"])
        log_xml = ET.XML(log_item["Msg"])
        NasTypePattern = re.compile(r": (.*) \(0x[\da-fA-F]+\)$")

        nas_type = ""
        for val in log_xml.iter("field"):
            if val.get("name") in {
                "gsm_a.dtap.msg_mm_type",
                "gsm_a.dtap.msg_gmm_type",
                    "gsm_a.dtap.msg_sm_type"}:
                s = val.get("showname")
                nas_type = re.findall(NasTypePattern, s)[0]
                break
        # print nas_type

        # WCDMA Attach
        if nas_type == "Attach Request":
            start_span(
                self.__umts_attach,
                log_item,
                request=nas_type,
                response=None)
        elif nas_type in {"Attach Complete", "Attach Reject"}:
            if in_span(self.__umts_attach):
                end_span(self.__umts_attach, log_item)
                self.__umts_attach[-1].response = nas_type

        # WCDMA Routing Area Update
        if nas_type == "Routing Area Update Request":
            start_span(
                self.__umts_rau,
                log_item,
                request=nas_type,
                response=None)
        elif nas_type in {"Routing Area Update Complete", "Routing Area Update Reject"}:
            if in_span(self.__umts_rau):
                end_span(self.__umts_rau, log_item)
                self.__umts_rau[-1].response = nas_type

        # WCDMA Location Update
        if nas_type == "Location Updating Request":
            start_span(
                self.__umts_lu,
                log_item,
                request=nas_type,
                response=None)
        elif nas_type in {"Location Updating Accept", "Location Updating Reject"}:
            if in_span(self.__umts_lu):
                end_span(self.__umts_lu, log_item)
                self.__umts_lu[-1].response = nas_type

    def __callback_wcdma_cell_id(self, event):
        log_item = event.data

        self.__last_normal_service = "WCDMA/%s" % log_item["PLMN"]

    def __callback_lte_nas_emm(self, event):
        log_item = event.data
        last_normal_service = self.__last_normal_service

        # Normal service span
        if log_item["EMM Substate"] == "EMM_REGISTERED_NORMAL_SERVICE":
            start_span(self.__lte_normal_service, log_item)
            self.__last_normal_service = "LTE/%s" % log_item["PLMN"]
        elif log_item["EMM Substate"] in {"Unknown", "Undefined"}:
            pass
        else:
            end_span(self.__lte_normal_service, log_item)
            # if self.__last_normal_service.startswith("LTE"):
            #     self.__last_normal_service = ""

        # PLMN service span
        if log_item["EMM Substate"] in {
            "EMM_DEREGISTERED_PLMN_SEARCH",
                "EMM_REGISTERED_PLMN_SEARCH"}:
            self.__start_plmn_search("LTE", last_normal_service, log_item)
        elif log_item["EMM Substate"] == "EMM_REGISTERED_NORMAL_SERVICE":
            self.__end_plmn_search(log_item)

    def __callback_lte_nas(self, event):
        log_item = event.data
        # log_xml = ET.fromstring(log_item["Msg"])
        log_xml = ET.XML(log_item["Msg"])
        NasTypePattern = re.compile(r": (.*) \(0x[\da-fA-F]+\)")

        nas_type = ""
        for val in log_xml.iter("field"):
            if val.get("name") in {
                "nas_eps.nas_msg_emm_type",
                    "nas_eps.nas_msg_esm_type"}:
                s = val.get("showname")
                nas_type = re.findall(NasTypePattern, s)[0]
                break
        # print nas_type

        # LTE Attach
        if nas_type in {"Attach request"}:
            start_span(
                self.__lte_attach,
                log_item,
                request=nas_type,
                response=None)
        elif nas_type in {"Attach complete", "Attach reject"}:
            if in_span(self.__lte_attach):
                end_span(self.__lte_attach, log_item)
                self.__lte_attach[-1].response = nas_type

        # LTE Tracking Area Update
        if nas_type in {"Tracking area update request"}:
            start_span(
                self.__lte_tau,
                log_item,
                request=nas_type,
                response=None)
        elif nas_type in {"Tracking area update complete", "Tracking area update reject"}:
            if in_span(self.__lte_tau):
                end_span(self.__lte_tau, log_item)
                self.__lte_tau[-1].response = nas_type

        if nas_type == "Activate default EPS bearer context request":
            keys = (
                "qci",
                "delay_class",
                "traffic_class",
                "delivery_err_sdu",
                "traffic_hand_pri",
                "traffic_hand_pri",
                "traffic_hand_pri",
                "apn_ambr_dl_ext",
                "apn_ambr_ul_ext",
                "apn_ambr_dl_ext2",
                "apn_ambr_ul_ext2")
            info = dict([(k, None) for k in keys])
            Pattern1 = re.compile(r": (.*) \((\d+)\)$")
            Pattern2 = re.compile(r": (\d+ \w+)$")
            for val in log_xml.iter("field"):
                s = val.get("showname")
                if val.get("name") == "nas_eps.emm.qci":
                    info["qci"] = re.findall(Pattern1, s)[0][0]
                elif val.get("name") == "gsm_a.gm.sm.qos.delay_cls":
                    info["delay_class"] = re.findall(Pattern1, s)[0][0]
                elif val.get("name") == "gsm_a.gm.sm.qos.traffic_cls":
                    info["traffic_class"] = "%s (%s)" % re.findall(
                        Pattern1, s)[0]
                elif val.get("name") == "gsm_a.gm.sm.qos.del_of_err_sdu":
                    info["delivery_err_sdu"] = "%s (%s)" % re.findall(Pattern1, s)[
                        0]
                elif val.get("name") == "gsm_a.gm.sm.qos.traff_hdl_pri":
                    info["traffic_hand_pri"] = "%s (%s)" % re.findall(Pattern1, s)[
                        0]
                elif val.get("name") == "gsm_a.gm.sm.qos.max_bitrate_downl_ext":
                    info["traffic_hand_pri"] = "%s (%s)" % re.findall(Pattern1, s)[
                        0]
                elif val.get("name") == "gsm_a.gm.sm.qos.max_bitrate_upl_ext":
                    info["traffic_hand_pri"] = "%s (%s)" % re.findall(Pattern1, s)[
                        0]
                elif val.get("name") == "nas_eps.emm.apn_ambr_dl_ext":
                    info["apn_ambr_dl_ext"] = re.findall(Pattern2, s)[0]
                elif val.get("name") == "nas_eps.emm.apn_ambr_ul_ext":
                    info["apn_ambr_ul_ext"] = re.findall(Pattern2, s)[0]
                elif val.get("name") == "nas_eps.emm.apn_ambr_dl_ext2":
                    info["apn_ambr_dl_ext2"] = re.findall(Pattern2, s)[0]
                elif val.get("name") == "nas_eps.emm.apn_ambr_ul_ext2":
                    info["apn_ambr_ul_ext2"] = re.findall(Pattern2, s)[0]
            info["last_lte_rrc_freq"] = self.__last_lte_rrc_freq
            self.__lte_tau_qos_info.append(info)

    def __callback_lte_rrc_ota(self, event):
        log_item = event.data
        if "Msg" not in log_item:
            return
        # log_xml = ET.fromstring(log_item["Msg"])
        log_xml = ET.XML(log_item["Msg"])

        is_sib1 = False
        is_sib6 = False
        is_rrc_conn_reconfig = False

        cell_info = {"plmn": None, "tac": None, "cell_id": None}
        if log_item["PDU Number"] == 2:  # BCCH_DL_SCH
            for val in log_xml.iter("field"):
                if val.get(
                        "name") == "lte-rrc.systemInformationBlockType1_element":
                    is_sib1 = True
                elif val.get("name") == "lte-rrc.sib6_element":
                    is_sib6 = True
                elif val.get("name") == "lte-rrc.plmn_Identity_element":
                    mcc_mnc = ""
                    for digit in val.iter("field"):
                        if digit.get("name") == "lte-rrc.MCC_MNC_Digit":
                            mcc_mnc += digit.get("show")
                    cell_info["plmn"] = mcc_mnc[0:3] + "-" + mcc_mnc[3:]
                elif val.get("name") == "lte-rrc.trackingAreaCode":
                    cell_info["tac"] = int(val.get("value"), base=16)
                elif val.get("name") == "lte-rrc.cellIdentity":
                    cell_info["cell_id"] = int(val.get("value"), base=16) / 16

        elif log_item["PDU Number"] == 6:  # LTE-RRC_DL_DCCH
            for val in log_xml.iter("field"):
                if val.get(
                        "name") == "lte-rrc.rrcConnectionReconfiguration_element":
                    is_rrc_conn_reconfig = True
                    break

        if is_sib1 or is_sib6 or is_rrc_conn_reconfig:
            Pattern1 = re.compile(r": (.*) \([-\d]+\)$")
            Pattern2 = re.compile(r": (.*)$")

        if is_sib1:
            s = "LTE/%(plmn)s-%(tac)d-%(cell_id)d" % cell_info
            self.__add_plmn_search_cell(s, log_item)
            info = {"subframeAssignment": None,
                    "specialSubframePatterns": None,
                    "si_WindowLength": None,
                    "systemInfoValueTag": None
                    }
            for attr in log_xml.iter("field"):
                ss = attr.get("showname")
                if attr.get("name") in (
                    "lte-rrc.subframeAssignment",
                    "lte-rrc.specialSubframePatterns",
                        "lte-rrc.si_WindowLength"):
                    info[attr.get("name")[8:]] = re.findall(Pattern1, ss)[0]
                elif attr.get("name") == "lte-rrc.systemInfoValueTag":
                    info[attr.get("name")[8:]] = re.findall(Pattern2, ss)[0]
            info["lte_rrc_freq"] = log_item["Freq"]
            self.__lte_tdd_config.append(info)

        if is_sib6:
            # Iter over all CarrierFreqUTRA_FDD elements
            for val in log_xml.iter("field"):
                if val.get("name") == "lte-rrc.CarrierFreqUTRA_FDD_element":
                    info = dict()
                    # Iter over all attrs
                    for attr in val.iter("field"):
                        s = attr.get("showname")
                        if attr.get("name") in (
                            "lte-rrc.threshX_High",
                            "lte-rrc.threshX_Low",
                                "lte-rrc.utra_q_RxLevMin"):
                            info[attr.get("name")[8:]] = re.findall(
                                Pattern1, s)[0]
                        elif attr.get("name") in ("lte-rrc.carrierFreq", "lte-rrc.cellReselectionPriority", "lte-rrc.p_MaxUTRA", "lte-rrc.q_QualMin"):
                            info[attr.get("name")[8:]] = re.findall(
                                Pattern2, s)[0]
                    info["lte_rrc_freq"] = log_item["Freq"]
                    self.__lte_cell_resel_to_umts_config.append(info)

        if is_rrc_conn_reconfig:
            # Find drx-Config setup
            for val in log_xml.iter("field"):
                if val.get(
                        "name") == "lte-rrc.drx_Config" and val.get("show") == "1":
                    info = {"shortDRX_Cycle": None, "drxShortCycleTimer": None}
                    for attr in val.iter("field"):
                        s = attr.get("showname")
                        if attr.get("name") in (
                            "lte-rrc.onDurationTimer",
                            "lte-rrc.drx_InactivityTimer",
                            "lte-rrc.drx_RetransmissionTimer",
                                "lte-rrc.shortDRX_Cycle"):
                            info[attr.get("name")[8:]] = re.findall(
                                Pattern1, s)[0]
                        elif attr.get("name") == "lte-rrc.drxShortCycleTimer":
                            info[attr.get("name")[8:]] = re.findall(
                                Pattern2, s)[0]
                    info["lte_rrc_freq"] = log_item["Freq"]
                    self.__lte_drx_config.append(info)
                    break
            self.__n_lte_rrc_reconfig += 1

        self.__last_lte_rrc_freq = log_item["Freq"]

    def __callback_lte_rrc_serv_cell_info(self, event):
        log_item = event.data

        if "MNC Digit" not in log_item:
            return

        if log_item["MNC Digit"] == 3:
            s = "LTE/%(MCC)03d-%(MNC)03d-%(TAC)d-%(Cell Identity)d" % log_item
        elif log_item["MNC Digit"] == 2:
            s = "LTE/%(MCC)03d-%(MNC)02d-%(TAC)d-%(Cell Identity)d" % log_item
        self.__add_plmn_search_cell(s, log_item)
