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

        self.register_kpi("Mobility","HO_FAILURE", self.__ho_sr_callback, 0)
        self.register_kpi("Mobility", "HO_TOTAL", self.__ho_sr_callback, 0)
        self.register_kpi("Mobility", "HO_SR", self.__ho_sr_callback, 0)

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

    def __ho_sr_callback(self, msg):
        # deal with RRC OTA

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

        return 0





