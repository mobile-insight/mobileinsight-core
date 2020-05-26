#!/usr/bin/python
# Filename: dedicated_bearer_sr_analyzer.py
"""
dedicated_bearer_sr_analyzer.py
An KPI analyzer to monitor and manage dedicated bearer setup success rate

Author: Zhehui Zhang
"""

__all__ = ["DedicatedBearerSrAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer


class DedicatedBearerSrAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to monitor and manage Dedicated EPS bearer setup success rate
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        # dedicated epx bearer may have qci: 1, 2, 3, 4, 65, 66, 75
        self.kpi_measurements = {'success_number': {'QCI1': 0, 'QCI2': 0, 'QCI3': 0, 'QCI4': 0}, \
                                 'total_number': {'QCI1': 0, 'QCI2': 0, 'QCI3': 0, 'QCI4': 0}}
        self.current_kpi = {'QCI1': 0, 'QCI2': 0, 'QCI3': 0, 'QCI4': 0}

        for kpi in self.current_kpi:
            # self.register_kpi("KPI_Accessibility_DEDICATED_BEARER_SR_" + kpi, self.__esm_sr_callback)
            self.register_kpi("Accessibility", "DEDICATED_BEARER_SR_" + kpi + "_REQ", self.__esm_sr_callback)
        for kpi in self.current_kpi:
            # self.register_kpi("KPI_Accessibility_DEDICATED_BEARER_SR_" + kpi, self.__esm_sr_callback)
            self.register_kpi("Accessibility", "DEDICATED_BEARER_SR_" + kpi + "_SUC", self.__esm_sr_callback)
        for kpi in self.current_kpi:
            # self.register_kpi("KPI_Accessibility_DEDICATED_BEARER_SR_" + kpi, self.__esm_sr_callback)
            self.register_kpi("Accessibility", "DEDICATED_BEARER_SR_" + kpi + "_SR", self.__esm_sr_callback)

        self.type = None # record bearer qci for current bearer setup procedure

        # initilize kpi values
        self.__calculate_kpi()

        # add callback function
        self.add_source_callback(self.__esm_sr_callback)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE ESM messages.

        :param source: the trace source.
        :type source: trace collector
        """
        KpiAnalyzer.set_source(self,source)
        #enable LTE EMM logs
        source.enable_log("LTE_NAS_ESM_OTA_Incoming_Packet")
        source.enable_log("LTE_NAS_ESM_OTA_Outgoing_Packet")

    def __calculate_kpi(self):
        for type in self.current_kpi:
            if self.kpi_measurements['total_number'][type] != 0:
                self.current_kpi[type] = \
                    self.kpi_measurements['success_number'][type] / float(self.kpi_measurements['total_number'][type])
            else:
                self.current_kpi[type] = 0.00

    def __esm_sr_callback(self, msg):
        # deal with ESM OTA
        if msg.type_id == "LTE_NAS_ESM_OTA_Incoming_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                for proto in log_xml.iter('proto'):
                    if proto.get('name') == 'nas-eps':
                        act_bearer_flag = False
                        for field in proto.iter('field'):
                            # print ET.dump(field)
                            if field.get('name') == 'nas_eps.nas_msg_esm_type' and field.get('value') == 'c5':
                                act_bearer_flag = True
                            elif act_bearer_flag and field.get('name') == 'nas_eps.emm.qci':
                                if 0 < int(field.get('show')) < 5:
                                    self.type = 'QCI' + field.get('show')
                                    self.kpi_measurements['total_number'][self.type] += 1
                                    self.store_kpi("KPI_Accessibility_DEDICATED_BEARER_SR_" + self.type + "_REQ", \
                                                   str(self.kpi_measurements['total_number'][self.type]), log_item_dict['timestamp'])
                                else:
                                    self.log_warning('Unknown dedicated bearer qci: ', filed.get('show'))

        elif msg.type_id == "LTE_NAS_ESM_OTA_Outgoing_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                for proto in log_xml.iter('proto'):
                    if proto.get('name') == 'nas-eps':
                        for field in proto.iter('field'):
                            # print ET.dump(field)
                            if field.get('name') == 'nas_eps.nas_msg_esm_type' and field.get('value') == 'c6':
                                if self.type:
                                    self.kpi_measurements['success_number'][self.type] += 1
                                    self.__calculate_kpi()
                                    self.log_debug("KPI_DEDICATED_BEARER_SR: " + str(self.current_kpi))
                                    self.store_kpi("KPI_Accessibility_DEDICATED_BEARER_SR_" + self.type + "_SUC", \
                                                   str(self.kpi_measurements['success_number'][self.type]), log_item_dict['timestamp'])
                                    # self.store_kpi("KPI_Accessibility_DEDICATED_BEARER_SR_" + self.type, \
                                                   # '{:.2f}'.format(self.current_kpi[self.type]), log_item_dict['timestamp'])
                                    self.type = None





