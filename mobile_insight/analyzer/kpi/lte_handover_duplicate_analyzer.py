#!/usr/bin/python
# Filename: lte_handover_duplicate_analyzer.py
"""
A 4G analyzer to get RRC, RLC and PDCP layer information to analyze the phenomenon and cause of duplicate transmission after handover in LTE

Author: Qianru Li, Yuanjie Li
"""

try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET
import timeit
import time

from ..analyzer import *
from .kpi_analyzer import KpiAnalyzer

__all__=["LteHandoverDuplicateAnalyzer"]

class L2Bearer:
    def __init__(self,cfgidx):
        self.cfgIdx = cfgidx
        self.pdcp_dic = {}
        self.rlc_dic = {}
        self.rlc_sequence = []
        self.rlc_ack_lst = []

        self.pdcp_sequence = []

        self.highest_pdcp = None
        self.cons_valid = 0

        self.cons_invalid = 0

        self.last_pdcp = None

        self.stop_pdcp = False

        self.last_invalid = -1
        self.time_cnt = 0

    def add_pdcp_dl_data_pkt(self, record, hpdcp):
        pdcp_sys_fn = record['Sys FN']
        pdcp_sub_fn = record['Sub FN']
        seq_num = record['SN']
        systime = pdcp_sys_fn * 10 + pdcp_sub_fn

        hol_delay = None # Return value: HOL delay (if any)

        if self.last_pdcp != None:
            if self.last_pdcp[0] != systime:
                i = (self.last_pdcp[0]+1)%10240
                while i!=systime:
                    self.pdcp_dic[i]=self.last_pdcp[1]
                    i = (i+1)%10240

        self.pdcp_dic[systime] = seq_num
        self.last_pdcp = [systime,seq_num]
        if record['Valid PDU'] == 'Yes':
            if self.cons_valid == 0:
                # print "First valid, {}, {}, {},".format(self.cfgIdx,seq_num,systime)
                if self.time_cnt > 0:
                    hol_delay = self.time_cnt + (systime - self.last_invalid + 10240) % 10240

                # print "HOL delay", str(hol_delay)

                self.time_cnt = 0
                self.last_invalid = -1

            self.pdcp_sequence.append([systime, seq_num])
            self.cons_valid += 1

        elif (hpdcp == None or seq_num <= hpdcp) and self.cons_valid < 50:
            if self.cons_valid > 0 and self.stop_pdcp == False:
                # print "Stop here,{},".format(self.cfgIdx)
                self.stop_pdcp = True

            elif self.stop_pdcp == False:
                if self.last_invalid < 0:
                    self.time_cnt = 1

                else:
                    self.time_cnt += (systime - self.last_invalid + 10240) % 10240

                self.last_invalid = systime

                # print "Invalid sn, {}, {}, {},".format(self.cfgIdx, seq_num, systime) 

        return hol_delay

    def add_pdcp_ho(self, systime):
        if self.last_pdcp != None:
            if self.last_pdcp[0] != systime:
                i = (self.last_pdcp[0]+1)%10240
                while i!=systime:
                    self.pdcp_dic[i]=self.last_pdcp[1]
                    i = (i+1)%10240

    def add_rlc_ul_ack(self, record):
        ack_sn = int(''.join(record['SN'].split()).split('=')[1])
        self.rlc_ack_lst.append([record['sys_fn'],record['sub_fn'],ack_sn])

    def add_rlc_dl_data_pkt(self, record):
        self.rlc_dic[record['SN']] = record['sys_fn']*10+record['sub_fn']
        try:
            self.rlc_sequence.append([record['sys_fn']*10+record['sub_fn'], record['SN'], record['FI'], record['RLC DATA LI']])
        except KeyError:
            self.rlc_sequence.append([record['sys_fn']*10+record['sub_fn'], record['SN'], record['FI'], ()])

    def get_rlc_ack(self,bearid):
        last_acks = []

        # print "highest_pdcp,{},{},".format(bearid, self.highest_pdcp)

        for i in range(0,3):
            if self.rlc_ack_lst != []:
                last_acks.append(self.rlc_ack_lst.pop())

        i = len(self.rlc_sequence) - 1
        j = len(self.pdcp_sequence) - 1

        for last_ack in last_acks:
            get_time = None
            try:
                while i >= 0:
                    # print "rlc dl, {},{},{},{}".format(self.rlc_sequence[i][0],self.rlc_sequence[i][1], self.rlc_sequence[i][2],self.rlc_sequence[i][3])
                    # print "rlc dl, {},{}".format(self.rlc_sequence[i][0], self.rlc_sequence[i][1])
                    if (self.rlc_sequence[i][0]<last_ack[0]*10+last_ack[1]) and self.rlc_sequence[i][1]<last_ack[2]:
                        get_time = self.rlc_sequence[i][0]
                        break
                    i-=1

                get_pdcp_sn = self.pdcp_dic[get_time]
                # print "last ack,{},{},{},".format(bearid,get_pdcp_sn,last_ack[0]*10+last_ack[1])

                while j >= 0 and self.pdcp_sequence[j][0] > get_time:
                    # print "received pdcp, {},{},{}".format(bearid, self.pdcp_sequence[j][1], self.pdcp_sequence[j][0])
                    j-=1
            except KeyError:
                continue

# class ho_record:
#     def __init__(self):
#         self.ho_time     = None
#         self.last_acks     = []
#         self.dup_pdcps     = []


class LteHandoverDuplicateAnalyzer(KpiAnalyzer):
    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.bearer_dic = {}

        self.handover_time = None

        self.pdcp_highest = {}        # key is bearer id
        self.rlc_sequence_dic = {}     # key is bearer id

        self.__flag = False
        self.__print = 0

        self.register_kpi("Mobility", "HANDOVER_HOL", self.__msg_callback, 0)
    
    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        KpiAnalyzer.set_source(self,source)
        # Enable RRL-layer downlink logs
        source.enable_log("LTE_RRC_OTA_Packet")
        # Enable RLC-layer downlink logs
        source.enable_log("LTE_RLC_DL_AM_All_PDU")
        # Enable RLC-layer uplink logs
        source.enable_log("LTE_RLC_UL_AM_All_PDU")
        # Enable PDCP-layer downlink logs
        source.enable_log("LTE_PDCP_DL_Cipher_Data_PDU")

    def __msg_callback(self,msg):
        log_item = msg.data.decode()
        if msg.type_id == "LTE_RRC_OTA_Packet" and 'Msg' in log_item:
            log_xml = ET.XML(log_item['Msg'])
            xml_msg = Event(log_item['timestamp'],msg.type_id,log_xml)
            for field in xml_msg.data.iter('field'):
                if field.get('name') == 'lte-rrc.mobilityControlInfo_element':
                    self.__flag = True
                    break

                if field.get('name') == 'lte-rrc.rrcConnectionReconfigurationComplete_element' and self.__flag:
                    self.__flag = False
                    # print "ho_time,{},".format(self.handover_time)
                    # ho_record.append([0,self.handover_time])
                    for bearid in self.bearer_dic:
                        self.bearer_dic[bearid].get_rlc_ack(bearid)
                        self.pdcp_highest[bearid] = self.bearer_dic[bearid].highest_pdcp
                    self.bearer_dic.clear()

        if msg.type_id == 'LTE_PDCP_DL_Cipher_Data_PDU':
            records = log_item['Subpackets'][0]['PDCPDL CIPH DATA']
            for record in records:
                cfgIdx = record['Cfg Idx']
                pdcp_sys_fn = record['Sys FN']
                pdcp_sub_fn = record['Sub FN']
                seq_num = record['SN']
                systime = pdcp_sys_fn * 10 + pdcp_sub_fn
                if cfgIdx > 30:
                    if self.__flag:
                        for bearid in self.bearer_dic:
                            self.bearer_dic[bearid].add_pdcp_ho(systime)
                            if self.bearer_dic[bearid].last_pdcp != None:
                                self.bearer_dic[bearid].highest_pdcp = self.bearer_dic[bearid].last_pdcp[1]
                        self.handover_time = systime

                else:
                    hol_delay = None
                    if self.__flag and systime == self.handover_time:
                        self.bearer_dic[cfgIdx].highest_pdcp = seq_num 
                    if cfgIdx not in self.bearer_dic:
                        self.bearer_dic[cfgIdx] = L2Bearer(cfgIdx)
                    if cfgIdx not in self.pdcp_highest:
                        hol_delay = self.bearer_dic[cfgIdx].add_pdcp_dl_data_pkt(record, None)
                    else:
                        hol_delay = self.bearer_dic[cfgIdx].add_pdcp_dl_data_pkt(record, self.pdcp_highest[cfgIdx])
                    
                    if hol_delay:
                        # self.log_info("Handover HOL delay: "+str(hol_delay))
                        ho_kpi = {'HOL delay (ms)': hol_delay}
                        # self.upload_kpi("KPI.Mobility.HOL_BLOCKING", ho_kpi)
                        # self.broadcast_info('HOL_BLOCKING', ho_kpi)
                        self.store_kpi("KPI_Mobility_HANDOVER_HOL", str(hol_delay), log_item['timestamp'])

        if msg.type_id == "LTE_RLC_DL_AM_All_PDU":
            records = log_item['Subpackets'][0]['RLCDL PDUs']
            for record in records:
                cfgIdx = record['rb_cfg_idx']
                if record['Status'] == 'PDU DATA' and cfgIdx<30:
                    # print record
                    if cfgIdx not in self.bearer_dic:
                        self.bearer_dic[cfgIdx] = L2Bearer(cfgIdx)
                    self.bearer_dic[cfgIdx].add_rlc_dl_data_pkt(record)

        if msg.type_id == "LTE_RLC_UL_AM_All_PDU":
            # print log_item
            records = log_item['Subpackets'][0]['RLCUL PDUs']
            for record in records:
                cfgIdx = record['rb_cfg_idx']
                if record['PDU TYPE'] == 'RLCUL CTRL' and cfgIdx<30 and 'SN' in record:
                    # print record
                    if cfgIdx not in self.bearer_dic:
                        self.bearer_dic[cfgIdx] = L2Bearer(cfgIdx)
                    self.bearer_dic[cfgIdx].add_rlc_ul_ack(record)