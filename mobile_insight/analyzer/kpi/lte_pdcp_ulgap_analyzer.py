#!/usr/bin/python
# Filename: lte_pdcp_gap_analyzer.py
"""
A 4G PDCP analyzer to calculate the number of lost PDCP packets

Author: Zhehui Zhang

Output: <timestamp>    <lost pdcp pkts> <lost pdcp pkts within one msg>
"""

from .kpi_analyzer import KpiAnalyzer
import time

__all__ = ["LtePdcpUlGapAnalyzer"]

class LtePdcpUlGapAnalyzer(KpiAnalyzer):
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
        self.last_fn_sfn = {}
        self.lost_record = {}

        self.pkt_total = 0
        self.register_kpi("Wireless", "UL_PDCP_LOSS", self.__msg_callback, 0)

    def set_source(self, source):
        KpiAnalyzer.set_source(self, source)

        source.enable_log("LTE_PDCP_UL_Cipher_Data_PDU")


    def __msg_callback(self, msg):
        if msg.type_id == "LTE_PDCP_UL_Cipher_Data_PDU":
            self._callback_pdcp_ul(msg)


    def _callback_pdcp_ul(self, msg):
        log_item = msg.data.decode()
        timestamp = log_item['timestamp'].timestamp()
        # timestamp = float(msg.timestamp)
        # self.log_info('timestamp ' + str(msg.timestamp)
        #              +" self.time_old: " + str(self.time_old))
        # timestamp = time.time()
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
                    bcast_total = float(self.lost_cnt_one_msg) / self.pkt_total
                bcast['PDCP gap ratio'] = bcast_total

                # self.broadcast_info('PDCP_GAP', bcast)
                if bcast_total > 0:
                    self.store_kpi("KPI_Wireless_UL_PDCP_LOSS", '{:.2f}'.format(bcast_total), log_item['timestamp'])

                self.lost_record[self.time_old] = self.lost_cnt
            # self.time_old = timestamp
            self.time_old = timestamp
            self.lost_cnt = 0 
            self.lost_cnt_one_msg = 0   
            self.pkt_total = 0        

        
        records = log_item['Subpackets'][0]['PDCPUL CIPH DATA']
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
                self.last_fn_sfn[cfgIdx] = pdcp_sys_fn*10+pdcp_sub_fn
                continue

            if (self.last_sn_one_msg[cfgIdx] + 1) % 4096 < pdcp_sn:
                self.lost_cnt_one_msg += (pdcp_sn - self.last_sn_one_msg[cfgIdx] + 4095) % 4096
                ts_str = str(log_item['timestamp'])[-15:]
                ts = ts_str.split(':')
                ts_seconds = int(ts[0]) * 3600 + int(ts[1]) * 60 + float(ts[2])
                # print 'PDCP loss:', log_item['timestamp'], ts_seconds, (pdcp_sn - self.last_sn_one_msg[cfgIdx] + 4095) % 4096, pdcp_sn, self.last_sn_one_msg[cfgIdx], 'SYS-FN', pdcp_sys_fn*10 + pdcp_sub_fn, self.last_fn_sfn[cfgIdx]
            self.last_sn_one_msg[cfgIdx] = pdcp_sn
            self.last_fn_sfn[cfgIdx] = pdcp_sys_fn*10+pdcp_sub_fn

        self.last_sn_one_msg.clear()
        self.last_fn_sfn.clear()
        # self.lost_cnt_one_msg = 0
