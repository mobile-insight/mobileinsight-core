#!/usr/bin/python
# Filename: ip_dl_tput_analyzer.py
"""
ip_dl_tput_analyzer.py
An KPI analyzer to monitor IP throughput from PDCP logs
Record the interval of the video frames

Author: Zhehui Zhang
"""

__all__ = ["IpDlTputAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer
from heapq import *

class IpDlTputAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to monitor and manage uplink latency breakdown
    """
    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)
        self.rlc_buffer_heap = []
        self.cur_ts = None
        self.cum_bytes = 0

        self.last_dl_pdu = None
        self.last_ul_pdu = None

        self.register_kpi("Integrity", "DL_TPUT", self.__msg_callback, 0)

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        KpiAnalyzer.set_source(self, source)

        # RLC logs
        # source.enable_log("LTE_RLC_DL_AM_All_PDU")
        source.enable_log("LTE_PDCP_DL_Cipher_Data_PDU")
        # source.enable_log("LTE_PDCP_UL_Cipher_Data_PDU")

    def time_diff(self, pdu1, pdu2):
        return (pdu2['Sys FN'] * 10 + pdu2['Sub FN']) - (pdu1['Sys FN'] * 10 + pdu1['Sub FN'])

    def __msg_callback(self, msg):
        # if msg.type_id == "LTE_RLC_DL_AM_All_PDU":
            # pass
            # log_item = msg.data.decode()
            # print log_item
            # if log_item.has_key('Subpackets'):
            #     subPkt = log_item['Subpackets'][0]
            #     listPDU = subPkt['RLCDL PDUs']
            #     for pduItem in listPDU:
            #         if pduItem['PDU TYPE'] == 'RLCDL DATA':
            #             hdr_len = int(pduItem['logged_bytes'])  # rlc_pdu_size = pdcp_pdu_size + rlc_hdr_len
            #             sdu_size = int(pduItem['pdu_bytes']) - hdr_len
            #             sys_fn = int(pduItem['sys_fn'])
            #             sub_fn = int(pduItem['sub_fn'])
        
        #TODO: implement UL thougput KPI
        if msg.type_id == "LTE_PDCP_UL_Cipher_Data_PDU":
            log_item = msg.data.decode()
            subPkt = log_item['Subpackets'][0]
            # print subPkt
            listPDU = subPkt['PDCPUL CIPH DATA']

            for pduItem in listPDU:
                if self.last_ul_pdu:
                  lap = self.time_diff(self.last_ul_pdu, pduItem)
                  self.last_ul_pdu = pduItem
                  # if lap > 5:
                    # print "Uplink", lap
                else:
                  self.last_ul_pdu = pduItem

        elif msg.type_id == "LTE_PDCP_DL_Cipher_Data_PDU":
            log_item = msg.data.decode()
            subPkt = log_item['Subpackets'][0]
            # print subPkt
            listPDU = subPkt['PDCPDL CIPH DATA']

            for pduItem in listPDU:
                # print pduItem
                sn = int(pduItem['SN'])
                sys_fn = int(pduItem['Sys FN'])
                sub_fn = int(pduItem['Sub FN'])
                pdu_size = int(pduItem['PDU Size'])
                self.cum_bytes += pdu_size


                if self.last_dl_pdu:
                  lap = self.time_diff(self.last_dl_pdu, pduItem)
                  self.last_dl_pdu = pduItem
                  # if lap > 5:
                    # print "Downlink", lap
                else:
                  self.last_dl_pdu = pduItem


                # self.pdcp_buffer.append([log_item['timestamp'], sys_fn, sub_fn, pdu_size, pdu_size])
            if self.cur_ts:
                lap = (log_item['timestamp'] - self.cur_ts).total_seconds()
                if lap > 1:
                    self.cur_ts = log_item['timestamp']
                    tput = self.cum_bytes*8/lap
                    self.cum_bytes = 0
                    self.store_kpi("KPI_Integrity_DL_TPUT", '{:.2f}'.format(tput), log_item['timestamp'])

            else:
                self.cur_ts = log_item['timestamp']

            
