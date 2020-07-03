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

        self.ho_req_flag = False
        self.ho_req_timestamp = None

        self.register_kpi("Mobility","HO_FAILURE", self.__ho_sr_callback, 0)
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

    def __ho_sr_callback(self, msg):
        # deal with RRC OTA

        if self.ho_req_flag and msg.type_id == "LTE_RRC_Serv_Cell_Info":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            self.ho_req_flag = False
            delta_time = (log_item_dict['timestamp']-self.ho_req_timestamp).total_seconds() * 1000
            if delta_time >= 0:
                self.store_kpi("KPI_Accessibility_HO_LATENCY",
                                       delta_time, log_item_dict['timestamp'])

        if msg.type_id == "LTE_RRC_OTA_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                for field in log_xml.iter('field'):
 
                    if field.get('name') == "lte-rrc.rrcConnectionReestablishmentRequest_element":
                        # <field name="lte-rrc.reestablishmentCause" pos="13" show="1" showname="reestablishmentCause: handoverFailure (1)" size="1" value="04" />
                        tag = 'failure'
                        for val in field.iter('field'):
                            if val.get('name') == 'lte-rrc.reestablishmentCause':
                                if int(val.get('show')) == 1:
                                    tag = 'handover_failure'
                                    self.store_kpi("KPI_Mobility_HO_FAILURE", str(self.kpi_measurements['failure_number']), log_item_dict['timestamp'])
                                

                    elif field.get('name') == "lte-rrc.mobilityControlInfo_element":
                        self.store_kpi("KPI_Mobility_HO_TOTAL", str(self.kpi_measurements['total_number']), log_item_dict['timestamp'])
                        self.ho_req_flag = True
                        self.ho_req_timestamp = log_item_dict['timestamp']

        return 0





