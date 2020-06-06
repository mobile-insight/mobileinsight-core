#!/usr/bin/python
# Filename: tau_sr_analyzer.py
"""
tau_sr_analyzer.py
An KPI analyzer to monitor and manage dedicated bearer setup success rate

Author: Zhehui Zhang
"""

__all__ = ["TauSrAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer

# full list refer to Table 9.9.3.9.1 in TS 24.301
EMM_cause = {'3': 'ILL_UE',
             '6': 'ILL_ME',
             '7': 'EPS_NOT_ALLOWED',
             '8': 'EPS_NONEPS_NOT_ALLOWED',
             '9': 'UE_ID_NOT_DERIVED',
             '10': 'IMPLIC_DETACHED',
             '11': 'PLMN_NOT_ALLOWED',
             '12': 'TA_NOT_ALLOWED',
             '13': 'ROAM_NOT_ALLOWED',
             '14': 'EPS_NOT_ALLOWED_PLMN',
             '15': 'NO_SUIT_CELL',
             '18': 'CS_DOMAIN_NOT_AVAIL',
             '19': 'ESM_FAILURE',
             '22': 'CONGESTION',
             '25': 'NOT_AUTH_CSG',
             '35': 'REQ_SERVICE_NOT_AUTH',
             '39': 'CS_NOT_AVAIL',
             '40': 'NO_EPS_ACTIVATED'}

class TauSrAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to monitor and manage tracking area update success rate
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.cell_id = None

        self.kpi_measurements = {'success_number': {'TOTAL': 0}, \
                                 'total_number': {'TOTAL': 0},\
                                 'reject_number': {}}
        for cause_idx in [3, 6, 7] + list(range(9, 16)) + [22, 25, 40]:
            self.kpi_measurements['reject_number'][EMM_cause[str(cause_idx)]] = 0

        # print self.kpi_measurements

        self.tau_req_flag = False # detect if it is under request state
        self.tau_req_timestamp = None # the timestamp for the TAU request

        # self.current_kpi = {'TOTAL': 0}

        self.register_kpi("Mobility", "TAU_SUC", self.__emm_sr_callback,
                          list(self.kpi_measurements['success_number'].keys()))
        self.register_kpi("Mobility", "TAU_SR", self.__emm_sr_callback)
        self.register_kpi("Mobility", "TAU_REQ", self.__emm_sr_callback,
                          list(self.kpi_measurements['total_number'].keys()))
        self.register_kpi("Mobility", "TAU_SR_LATENCY", self.__emm_sr_callback,
                          None)
        self.register_kpi("Retainability", "TAU_REJ", self.__emm_sr_callback,
                          list(self.kpi_measurements['reject_number'].keys()))

        # initilize kpi values
        # self.__calculate_kpi()

        # add callback function
        self.add_source_callback(self.__emm_sr_callback)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE ESM messages.

        :param source: the trace source.
        :type source: trace collector
        """
        KpiAnalyzer.set_source(self,source)
        #enable LTE EMM logs
        source.enable_log("LTE_NAS_EMM_OTA_Incoming_Packet")
        source.enable_log("LTE_NAS_EMM_OTA_Outgoing_Packet")

    def __calculate_kpi(self):
        for type in self.current_kpi:
            if self.kpi_measurements['total_number'][type] != 0:
                self.current_kpi[type] = \
                    self.kpi_measurements['success_number'][type] / float(self.kpi_measurements['total_number'][type])
            else:
                self.current_kpi[type] = 0.00

    def __clear_counters(self):
        for key, value in self.kpi_measurements.items():
            if type(value) == type(1):
                self.kpi_measurements[key] = 0
            else:
                for sub_key, sub_value in value.items():
                    value[sub_key] = 0

    def __emm_sr_callback(self, msg):

        # print 'log'

        cell_id = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_id()
        if cell_id != self.cell_id:
            self.cell_id = cell_id
            self.__clear_counters()

        if msg.type_id == "LTE_NAS_EMM_OTA_Incoming_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            # print log_item_dict
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                # print ET.dump(log_xml)
                for proto in log_xml.iter('proto'):
                    if proto.get('name') == 'nas-eps':
                        for field in proto.iter('field'):
                            # '49' indicates Tracking area update accept
                            if field.get('name') == 'nas_eps.nas_msg_emm_type' and field.get('value') == '49' and self.tau_req_flag:
                                self.kpi_measurements['success_number']['TOTAL'] += 1

                                # self.__calculate_kpi()
                                # self.log_info("TAU_SR: " + str(self.kpi_measurements))
                                self.store_kpi("KPI_Mobility_TAU_SUC",
                                            self.kpi_measurements['success_number'], log_item_dict['timestamp'])
                                upload_dict = {
                                    'total_number': self.kpi_measurements['total_number']['TOTAL'],
                                    'success_number': self.kpi_measurements['success_number']['TOTAL']}
                                self.upload_kpi('KPI.Mobility.TAU_SR', upload_dict)
                                self.tau_req_flag = False

                                # TAU latency
                                delta_time = (log_item_dict['timestamp']-self.tau_req_timestamp).total_seconds()
                                if delta_time >= 0:
                                    upload_dict = {'latency': delta_time}
                                    self.upload_kpi("KPI.Mobility.TAU_SR_LATENCY", upload_dict)

                            # '4b' indicates Tracking area update reject
                            elif field.get('name') == 'nas_eps.nas_msg_emm_type' and field.get('value') == '4b' and self.tau_req_flag:
                                for child_field in proto.iter('field'):
                                    if child_field.get('name') == 'nas_eps.emm.cause':
                                        cause_idx = str(child_field.get('show'))
                                        if cause_idx in EMM_cause:
                                            self.kpi_measurements['reject_number'][EMM_cause[cause_idx]] += 1
                                            # self.log_info("TAU_SR: " + str(self.kpi_measurements))
                                            self.store_kpi("KPI_Retainability_TAU_REJ",
                                                           self.kpi_measurements['reject_number'], log_item_dict['timestamp'])
                                            upload_dict = {
                                                'total_number': self.kpi_measurements['total_number']['TOTAL'],
                                                'reject_number': self.kpi_measurements['reject_number']}
                                            # self.upload_kpi('KPI.Retainability.RRC_AB_REL', upload_dict, log_item_dict['timestamp'])
                                            self.upload_kpi('KPI.Retainability.TAU_REJ', upload_dict)
                                        else:
                                            self.log_warning("Unknown EMM cause for TAU reject: " + cause_idx)
                                        self.tau_req_flag = False

        elif msg.type_id == "LTE_NAS_EMM_OTA_Outgoing_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            # print log_item_dict
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                # print ET.dump(log_xml)
                for proto in log_xml.iter('proto'):
                    if proto.get('name') == 'nas-eps':
                        for field in proto.iter('field'):
                            # '48' indicates Tracking area update request
                            if field.get('name') == 'nas_eps.nas_msg_emm_type' and field.get('value') == '48':
                                self.kpi_measurements['total_number']['TOTAL'] += 1
                                # self.log_info("TAU_SR: " + str(self.kpi_measurements))
                                self.tau_req_flag = True
                                self.tau_req_timestamp = log_item_dict['timestamp']
                                self.store_kpi("KPI_Mobility_TAU_REQ",
                                               self.kpi_measurements['total_number'], log_item_dict['timestamp'])






