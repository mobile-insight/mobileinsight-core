#!/usr/bin/python
# Filename: lte_handover_disruption_analyzer.py
"""
A 4G analyzer to get RRC and PDCP layer information to calculate ul/dl disruption time

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

__all__=["LteHandoverDisruptionAnalyzer"]

class LteHandoverDisruptionAnalyzer(KpiAnalyzer):
    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.__handover_state = 0
        # 0: wait for handover command
        # 1: wait for handover time (pdcp dl 33)
        # 2: wait for uplink data after handover (pdcp ul 33) / downlink data after handover (pdcp dl != 33)

        self.handover_time = None
        self.ul_time = None
        self.dl_time = None
        self.destination_cell =  None

        self.attributes = ["handover_time", "destination_cell", "uplink_disruption", "downlink_disruption"]

        self.register_kpi("Mobility", "HANDOVER_LATENCY", self.__msg_callback, 0)

    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        KpiAnalyzer.set_source(self,source)
        # Enable RRC-layer downlink logs
        source.enable_log("LTE_RRC_OTA_Packet")
        

        # Enable PDCP-layer downlink logs
        source.enable_log("LTE_PDCP_DL_Cipher_Data_PDU")
        # Enable PDCP-layer uplink logs
        source.enable_log("LTE_PDCP_UL_Cipher_Data_PDU")
        source.enable_log("LTE_RLC_DL_AM_All_PDU")

    def __msg_callback(self,msg):
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)

        if msg.type_id == "LTE_RRC_OTA_Packet" and 'Msg' in log_item_dict:
            log_xml = ET.XML(log_item_dict['Msg'])
            for field in log_xml.iter('field'):
                if field.get('name') == 'lte-rrc.mobilityControlInfo_element':
                    # Get destination cell ID
                    for item in field.iter('field'):
                        if item.get('name') ==  'lte-rrc.targetPhysCellId':
                            self.destination_cell = item.get('show')
                            break
                    self.__handover_state = 1
                    self.handover_time = None
                    self.ul_time = None
                    self.dl_time = None
                    break


        if msg.type_id == 'LTE_PDCP_DL_Cipher_Data_PDU' and self.__handover_state == 1:
            records = log_item['Subpackets'][0]['PDCPDL CIPH DATA']
            for record in records:
                cfgIdx = record['Cfg Idx']
                pdcp_sys_fn = record['Sys FN']
                pdcp_sub_fn = record['Sub FN']
                # seq_num = record['SN']
                systime = pdcp_sys_fn * 10 + pdcp_sub_fn
                if cfgIdx > 30:
                    # if self.__handover_state == 1:
                    self.handover_time = systime
                    self.__handover_state = 2

        if msg.type_id == "LTE_RLC_DL_AM_All_PDU" and self.__handover_state == 2 and self.dl_time == None:
            records = log_item['Subpackets'][0]['RLCDL PDUs']
            for record in records:
                systime = record['sys_fn']*10+record['sub_fn']
                if record['Status'] == 'PDU DATA' and (systime - self.handover_time >= 20 or (systime < 100 and self.handover_time > 10140)):
                    self.dl_time = systime
                    if self.ul_time != None:
                        self.__compute_disruption(log_item['timestamp'])
                    break

        if msg.type_id == 'LTE_PDCP_UL_Cipher_Data_PDU' and self.__handover_state == 2 and self.ul_time == None:
            records = log_item['Subpackets'][0]['PDCPUL CIPH DATA']
            for record in records:
                cfgIdx = record['Cfg Idx']
                pdcp_sys_fn = record['Sys FN']
                pdcp_sub_fn = record['Sub FN']

                systime = pdcp_sys_fn * 10 + pdcp_sub_fn
                if cfgIdx > 30 and (systime - self.handover_time >= 10 or (systime < 100 and self.handover_time > 10150)) :
                    self.ul_time = systime
                    if self.dl_time != None:
                        self.__compute_disruption(log_item['timestamp'])
                    break

    def __compute_disruption(self, ts):
        ul_dis = (self.ul_time - self.handover_time + 10240) % 10240
        dl_dis = (self.dl_time - self.handover_time + 10240) % 10240
        # self.log_info("Handover: {},{},{},{}".format(self.destination_cell, self.handover_time, ul_dis, dl_dis)) 
        self.__handover_state = 0

        # Upload the KPIs
        kpi = {}
        kpi['handover_time'] = str(self.handover_time)
        kpi['destination_cell'] = str(self.destination_cell)
        kpi['uplink_disruption'] = str(ul_dis)
        kpi['downlink_disruption'] = str(dl_dis)
        latency = max(ul_dis, dl_dis)
        self.broadcast_info('HANDOVER_LATENCY', kpi)
        self.store_kpi("KPI_Mobility_HANDOVER_LATENCY", str(latency), ts)



