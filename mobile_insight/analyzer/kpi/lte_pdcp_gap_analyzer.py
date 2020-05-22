#!/usr/bin/python
# Filename: lte_pdcp_gap_analyzer.py
"""
A 4G PDCP analyzer to calculate the number of lost PDCP packets

Author: Qianru Li

Output: <timestamp>    <lost pdcp pkts> <lost pdcp pkts within one msg>
"""

from xml.dom import minidom
import time

from .kpi_analyzer import KpiAnalyzer

__all__ = ["LtePdcpGapAnalyzer"]

class LtePdcpGapAnalyzer(KpiAnalyzer):
    def __init__(self):
        KpiAnalyzer.__init__(self)
        self.add_source_callback(self.__msg_callback)

        # self.time_old     = 0
        self.time_old = None
        self.lost_cnt     = 0
        self.lost_cnt_one_msg = 0
        # self.last_sn     = -1
        self.last_sn    = {}
        self.last_sn_one_msg = {}
        self.lost_record = {}

        self.pkt_total = 0

        self.register_kpi("Wireless", "DL_PDCP_LOSS", self.__msg_callback, 0)

    def set_source(self, source):
        KpiAnalyzer.set_source(self, source)

        source.enable_log("LTE_PDCP_DL_Cipher_Data_PDU")


    def __msg_callback(self, msg):
        if msg.type_id == "LTE_PDCP_DL_Cipher_Data_PDU":
            self._callback_pdcp_dl(msg)


    def _callback_pdcp_dl(self, msg):
        # timestamp = float(msg.timestamp)
        # self.log_info('timestamp ' + str(msg.timestamp)
        #              +" self.time_old: " + str(self.time_old))
        # timestamp = time.time()
        log_item = msg.data.decode()
        timestamp = log_item['timestamp'].timestamp()
        # self.log_info('timestamp ' + str(msg.timestamp)
                     # +" self.time_old: " + str(self.time_old))
        # if timestamp != self.time_old:
        if not self.time_old:
            self.time_old = timestamp
        if timestamp - self.time_old >= 1:
            if self.lost_cnt > 0:
                # self.log_info(str(self.time_old) + " " + str(self.lost_cnt) + ' ' + str(self.lost_cnt_one_msg))
                
                # self.log_info("PDCP packet loss: "+ str(self.lost_cnt_one_msg / (timestamp - self.time_old) ) + " pkt/s")

                bcast = {}
                bcast['PDCP gap'] = self.lost_cnt_one_msg / (timestamp - self.time_old)

                if self.pkt_total == 0:
                    bcast_total = 0.0
                else:
                    bcast_total = float(self.lost_cnt_one_msg) / self.pkt_total * 100
                bcast['PDCP gap ratio'] = bcast_total

                # self.broadcast_info('PDCP_GAP', bcast)
                # self.upload_kpi('KPI.Wireless.PDCP_LOSS', bcast)
                if bcast_total > 0:
                    # self.log_info(str(bcast_total))
                    self.store_kpi("KPI_Wireless_DL_PDCP_LOSS", '{:.2f}'.format(bcast_total), log_item['timestamp'])

                self.lost_record[self.time_old] = self.lost_cnt
            # self.time_old = timestamp
            self.time_old = timestamp
            self.lost_cnt = 0 
            self.lost_cnt_one_msg = 0   
            self.pkt_total = 0        

        records = log_item['Subpackets'][0]['PDCPDL CIPH DATA']
        # self.log_info(str(log_item))
        for record in records:
            cfgIdx = record['Cfg Idx']
            # if  self.last_sn
            pdcp_sys_fn = record['Sys FN']
            pdcp_sub_fn = record['Sub FN']
            pdcp_sn = record["SN"]

            self.pkt_total += 1

            if cfgIdx not in self.last_sn:
                self.last_sn[cfgIdx] = pdcp_sn
                continue

            if (self.last_sn[cfgIdx] + 1) % 4096 < pdcp_sn:
                self.lost_cnt += (pdcp_sn - self.last_sn[cfgIdx] + 4095) % 4096
            self.last_sn[cfgIdx] = pdcp_sn

            if cfgIdx not in self.last_sn_one_msg:
                self.last_sn_one_msg[cfgIdx] = pdcp_sn
                continue

            if (self.last_sn_one_msg[cfgIdx] + 1) % 4096 < pdcp_sn:
                self.lost_cnt_one_msg += (pdcp_sn - self.last_sn_one_msg[cfgIdx] + 4095) % 4096
            self.last_sn_one_msg[cfgIdx] = pdcp_sn

        self.last_sn_one_msg.clear()
        # self.lost_cnt_one_msg = 0
