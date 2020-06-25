#!/usr/bin/python
# Filename: service_req_sr_analyzer.py
"""
tau_sr_analyzer.py
An KPI analyzer to monitor and manage service request success rate

Author: Zhehui Zhang
"""

__all__ = ["ServiceReqSrAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer

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

class ServiceReqSrAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to monitor and manage tracking area update success rate
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.cell_id = None

        self.kpi_measurements = {'success_number': {'TOTAL': 0}, \
                                 'total_number': {'TOTAL': 0}, \
                                 'reject_number': {}}
        for cause_idx in [3, 6, 7] + list(range(9, 16)) + [18, 22, 25, 39, 40]:
            self.kpi_measurements['reject_number'][EMM_cause[str(cause_idx)]] = 0

        # print self.kpi_measurements

        self.service_req_flag = False

        # initilize kpi values
        # self.__calculate_kpi()

        self.register_kpi("Accessibility", "SR_SUC", self.__emm_sr_callback,
                          list(self.kpi_measurements['success_number'].keys()))
        self.register_kpi("Accessibility", "SR_REQ", self.__emm_sr_callback,
                          list(self.kpi_measurements['total_number'].keys()))
        self.register_kpi("Retainability", "SR_REJ", self.__emm_sr_callback,
                          list(self.kpi_measurements['reject_number'].keys()))
        self.register_kpi("Accessibility", "SR_SR", self.__emm_sr_callback)

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
        source.enable_log("LTE_NAS_ESM_State")
        source.enable_log("LTE_NAS_EMM_OTA_Outgoing_Packet")

    def __calculate_kpi(self):
        for type in self.current_kpi:
            if self.kpi_measurements['total_number'][type] != 0:
                self.current_kpi[type] = \
                    self.kpi_measurements['success_number'][type] / float(self.kpi_measurements['total_number'][type])
            else:
                self.current_kpi[type] = 0.00

            # self.upload_kpi(type, self.current_kpi[type])

    def __clear_counters(self):
        for key, value in self.kpi_measurements.items():
            if type(value) == type(1):
                self.kpi_measurements[key] = 0
            else:
                for sub_key, sub_value in value.items():
                    value[sub_key] = 0

    def __emm_sr_callback(self, msg):

        cell_id = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_id()
        if cell_id != self.cell_id:
            self.cell_id = cell_id
            self.__clear_counters()

        if msg.type_id == "LTE_NAS_ESM_State":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if self.service_req_flag and int(log_item_dict["EPS bearer state"]) == 2:
                self.kpi_measurements['success_number']['TOTAL'] += 1
                self.service_req_flag = False
                # self.__calculate_kpi()
                # self.log_info("SERVICE_REQ_SR: " + str(self.current_kpi))
                upload_dict = {
                    'total_number': self.kpi_measurements['total_number']['TOTAL'],
                    'success_number': self.kpi_measurements['success_number']['TOTAL']}
                # self.upload_kpi('KPI.Accessibility.SR_SR', upload_dict)
                # self.log_info("SR_SR: " + str(self.kpi_measurements))
                self.store_kpi("KPI_Accessibility_SR_SUC", self.kpi_measurements['success_number'], log_item_dict['timestamp'])
                               # '{:.2f}'.format(self.current_kpi['TOTAL']), msg.timestamp)

        elif msg.type_id == "LTE_NAS_EMM_OTA_Incoming_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            # print log_item_dict
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                # print ET.dump(log_xml)
                for proto in log_xml.iter('proto'):
                    if proto.get('name') == 'nas-eps':
                        for field in proto.iter('field'):
                            # '4d' indicates Service request
                            if field.get('name') == 'nas_eps.nas_msg_emm_type' and field.get('value') == '4d' and self.service_req_flag:
                                for child_field in proto.iter('field'):
                                    if child_field.get('name') == 'nas_eps.emm.cause':
                                        cause_idx = str(child_field.get('show'))
                                        if cause_idx in EMM_cause:
                                            self.kpi_measurements['reject_number'][EMM_cause[cause_idx]] += 1
                                            # self.log_info("SR_SR: " + str(self.kpi_measurements))
                                            self.store_kpi("KPI_Retainability_SR_REJ",
                                                           self.kpi_measurements['reject_number'], log_item_dict['timestamp'])
                                            upload_dict = {
                                                'total_number': self.kpi_measurements['total_number']['TOTAL'],
                                                'reject_number': self.kpi_measurements['reject_number']}
                                            # self.upload_kpi('KPI.Retainability.SR_REJ', upload_dict)
                                            # self.log_info("SR_REJ: " + str(self.kpi_measurements))
                                        else:
                                            self.log_warning("Unknown EMM cause for SR reject: " + cause_idx)
                                        self.service_req_flag = False

        elif msg.type_id == "LTE_NAS_EMM_OTA_Outgoing_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                for proto in log_xml.iter('proto'):
                    if proto.get('name') == 'nas-eps':
                        for field in proto.iter('field'):
                            if field.get('name') == 'nas_eps.security_header_type' and field.get('value') == 'C':
                                self.kpi_measurements['total_number']['TOTAL'] += 1
                                self.service_req_flag = True
                                self.store_kpi("KPI_Accessibility_SR_REQ", self.kpi_measurements['total_number'], log_item_dict['timestamp'])






