#!/usr/bin/python
# Filename: rrc_sr_analyzer.py
"""
rrc_sr_analyzer.py
A KPI analyzer to monitor and manage RRC connection success rate

Author: Zhehui Zhang
"""

__all__ = ["HoSrAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer


class HoSrAnalyzer(KpiAnalyzer):
    """
    A KPI analyzer to monitor and manage RRC connection success rate
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.cell_id = None

        self.kpi_measurements = {'failure_number': 0, 'total_number': 0}

        self.ho_req_flag = 0
        self.ho_req_timestamp = None

        # data plane latency
        self.handover_time = None
        self.ul_time = None
        self.dl_time = None
        self.destination_cell =  None

        self.reest_flag = False

        self.register_kpi("Mobility","HO_FAILURE", self.__ho_sr_callback, 0)
        self.register_kpi("Mobility","HO_FAILURE_LATENCY", self.__ho_sr_callback, 0)
        self.register_kpi("Mobility", "HO_TOTAL", self.__ho_sr_callback, 0)
        self.register_kpi("Mobility", "HO_SR", self.__ho_sr_callback, 0)
        self.register_kpi("Accessibility", "HO_LATENCY", self.__ho_sr_callback, 0)

        # add callback function
        self.add_source_callback(self.__ho_sr_callback)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        KpiAnalyzer.set_source(self,source)
        #enable LTE RRC log
        source.enable_log("LTE_RRC_OTA_Packet")
        source.enable_log("LTE_RRC_Serv_Cell_Info")
        # Enable PDCP-layer downlink logs
        source.enable_log("LTE_PDCP_DL_Cipher_Data_PDU")
        # Enable PDCP-layer uplink logs
        source.enable_log("LTE_PDCP_UL_Cipher_Data_PDU")
        source.enable_log("LTE_RLC_DL_AM_All_PDU")

    def __ho_sr_callback(self, msg):
        # deal with RRC OTA

        if self.ho_req_flag == 1 and msg.type_id == "LTE_RRC_Serv_Cell_Info" and self.ho_req_timestamp:
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            self.ho_req_flag = False
            delta_time = (log_item_dict['timestamp']-self.ho_req_timestamp).total_seconds() * 1000
            if delta_time >= 0:
                self.store_kpi("KPI_Accessibility_HO_LATENCY",
                                       delta_time, log_item_dict['timestamp'])
            self.ho_req_timestamp = None

        if msg.type_id == "LTE_RRC_OTA_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                for field in log_xml.iter('field'):
 
                    if field.get('name') == "lte-rrc.rrcConnectionReestablishmentRequest_element":
                        # <field name="lte-rrc.reestablishmentCause" pos="13" show="1" showname="reestablishmentCause: handoverFailure (1)" size="1" value="04" />
                        tag = 'failure'
                        self.ho_req_flag = 0
                        for val in field.iter('field'):
                            if val.get('name') == 'lte-rrc.reestablishmentCause':
                                if int(val.get('show')) == 1:
                                    tag = 'handover_failure'
                                    self.store_kpi("KPI_Mobility_HO_FAILURE", str(self.kpi_measurements['failure_number']), log_item_dict['timestamp'])
                                    self.reest_flag = True
                                    break

                    elif self.reest_flag and field.get('name') == "lte-rrc.rrcConnectionReestablishmentComplete_element" and self.ho_req_timestamp:
                        delta_time = (log_item_dict['timestamp']-self.ho_req_timestamp).total_seconds() * 1000
                        if delta_time >= 0:
                            self.store_kpi("KPI_Mobility_HO_FAILURE_LATENCY",
                                                   delta_time, log_item_dict['timestamp'])
                        self.ho_req_timestamp = None
                        self.reest_flag = False
                        break

                                
                    elif field.get('name') == "lte-rrc.mobilityControlInfo_element":
                        self.store_kpi("KPI_Mobility_HO_TOTAL", str(self.kpi_measurements['total_number']), log_item_dict['timestamp'])
                        self.ho_req_flag = 1
                        self.ho_req_timestamp = log_item_dict['timestamp']
                        self.handover_time = None
                        self.ul_time = None
                        self.dl_time = None
                        break

        if msg.type_id == 'LTE_PDCP_DL_Cipher_Data_PDU' and self.ho_req_flag == 1:
            log_item = msg.data.decode()
            records = log_item['Subpackets'][0]['PDCPDL CIPH DATA']
            for record in records:
                cfgIdx = record['Cfg Idx']
                pdcp_sys_fn = record['Sys FN']
                pdcp_sub_fn = record['Sub FN']
                # seq_num = record['SN']
                systime = pdcp_sys_fn * 10 + pdcp_sub_fn
                if cfgIdx > 30:
                    # if self.ho_req_flag == 1:
                    self.handover_time = systime
                    self.ho_req_flag = 2

        if msg.type_id == "LTE_RLC_DL_AM_All_PDU" and self.ho_req_flag == 2 and self.dl_time == None:
            log_item = msg.data.decode()
            records = log_item['Subpackets'][0]['RLCDL PDUs']
            for record in records:
                systime = record['sys_fn']*10+record['sub_fn']
                if record['Status'] == 'PDU DATA' and (systime - self.handover_time >= 20 or (systime < 100 and self.handover_time > 10140)):
                    self.dl_time = systime
                    if self.ul_time != None:
                        self.__compute_disruption(log_item['timestamp'])
                    break

        if msg.type_id == 'LTE_PDCP_UL_Cipher_Data_PDU' and self.ho_req_flag == 2 and self.ul_time == None:
            log_item = msg.data.decode()
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
        self.ho_req_flag = 0

        # Upload the KPIs
        kpi = {}
        kpi['handover_time'] = str(self.handover_time)
        kpi['destination_cell'] = str(self.destination_cell)
        kpi['uplink_disruption'] = str(ul_dis)
        kpi['downlink_disruption'] = str(dl_dis)
        latency = max(ul_dis, dl_dis)
        self.broadcast_info('HANDOVER_LATENCY', kpi)
        self.store_kpi("KPI_Accessibility_HO_LATENCY", str(latency), ts)






