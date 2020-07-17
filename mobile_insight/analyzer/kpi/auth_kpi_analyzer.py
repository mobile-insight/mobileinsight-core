#!/usr/bin/python
# Filename: auth_kpi_analyzer.py
"""
auth_kpi_analyzer.py
An KPI analyzer to monitor authentication prcedure related KPIs

Author: Zhehui Zhang
"""

__all__ = ["AuthKpiAnalyzer"]

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
             '20': 'MAC_FAILURE',
             '21': 'SYNC_FAILURE',
             '22': 'CONGESTION',
             '25': 'NOT_AUTH_CSG',
             '26': 'NON_EPS_UNACCEPT',
             '35': 'REQ_SERVICE_NOT_AUTH',
             '39': 'CS_NOT_AVAIL',
             '40': 'NO_EPS_ACTIVATED'}

class AuthKpiAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to monitor and manage RRC connection success rate
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.cell_id = None

        self.kpi_measurements = {'success_number': {'TOTAL': 0}, \
                                 'total_number': {'TOTAL': 0},\
                                 'reject_number': {'TOTAL': 0},\
                                 'failure_number': {}} # auth rej is NTK -> UE, auth failure is UE->NTK
        # self.current_kpi = {'EMERGENCY': 0, 'NORMAL': 0, 'COMBINED': 0}

        # for cause_idx in [20]:
            # TODO: find cause for Auth rej
            # self.kpi_measurements['reject_number'][EMM_cause[str(cause_idx)]] = 0

        for cause_idx in [20, 21, 26]:
            self.kpi_measurements['failure_number'][EMM_cause[str(cause_idx)]] = 0

        self.register_kpi("Accessibility", "AUTH_SUC", self.__emm_sr_callback,
                          list(self.kpi_measurements['success_number'].keys()))
        self.register_kpi("Accessibility", "AUTH_REQ", self.__emm_sr_callback,
                          list(self.kpi_measurements['total_number'].keys()))
        self.register_kpi("Retainability", "AUTH_REJ", self.__emm_sr_callback,
                          list(self.kpi_measurements['reject_number'].keys()))
        self.register_kpi("Retainability", "AUTH_FAIL", self.__emm_sr_callback,
                          list(self.kpi_measurements['failure_number'].keys()))

        self.type = None # record attach casue of current attach procedure

        # initilize kpi values
        # self.__calculate_kpi()

        # add callback function
        self.add_source_callback(self.__emm_sr_callback)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE EMM messages.

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
        # deal with EMM OTA
        cell_id = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_id()
        if cell_id != self.cell_id:
            self.cell_id = cell_id
            self.__clear_counters()

        if msg.type_id == "LTE_NAS_EMM_OTA_Incoming_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                # ET.dump(log_xml)
                for proto in log_xml.iter('proto'):
                    if proto.get('name') == 'nas-eps':
                        for field in proto.iter('field'):

                            if field.get('name') == "nas_eps.nas_msg_emm_type":
                                # showing '82' indicates Auth acccept, referring to http://niviuk.free.fr/lte_nas.php
                                if field.get('show') == "82":
                                    self.kpi_measurements['total_number']['TOTAL'] += 1
                                    self.store_kpi("KPI_Accessibility_AUTH_REQ",
                                                   self.kpi_measurements['total_number'], log_item_dict['timestamp'])


                                # '84' indicates Auth reject
                                elif field.get('show') == '84':
                                    # for child_field in proto.iter('field'):
                                    #     if child_field.get('name') == 'nas_eps.emm.cause':
                                    #         cause_idx = str(child_field.get('show'))
                                    #         if cause_idx in EMM_cause:
                                    self.kpi_measurements['reject_number']['TOTAL'] += 1
                                    self.store_kpi("KPI_Retainability_AUTH_REJ",
                                                   self.kpi_measurements['reject_number'], msg.timestamp)
                                    upload_dict = {
                                        'total_number': self.kpi_measurements['total_number']['TOTAL'],
                                        'reject_number': self.kpi_measurements['reject_number']['TOTAL']}
                                    self.upload_kpi('KPI.Retainability.AUTH_REJ', upload_dict)

                                    success_number = upload_dict['total_number'] - upload_dict['reject_number'] - \
                                        sum(self.kpi_measurements['failure_number'].values())
                                    upload_dict = {
                                        'total_number': self.kpi_measurements['total_number']['TOTAL'],
                                        'success_number': success_number
                                    }
                                    self.upload_kpi('KPI.Accessibility.AUTH_SR', upload_dict)
                                            # else:
                                            #     self.log_warning("Unknown EMM cause: " + cause_idx)

        elif msg.type_id == "LTE_NAS_EMM_OTA_Outgoing_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                # ET.dump(log_xml)
                for field in log_xml.iter('field'):
                    # TODO: how to check it succeed?
                    if field.get('name') == "nas_eps.nas_msg_emm_type":
                        # '92' indicates Auth failure
                        if field.get('show') == '92':
                            for child_field in proto.iter('field'):
                                if child_field.get('name') == 'nas_eps.emm.cause':
                                    cause_idx = str(child_field.get('show'))
                                    if cause_idx in EMM_cause:
                                        self.kpi_measurements['failure_number'][EMM_cause[cause_idx]] += 1
                                        self.store_kpi("KPI_Retainability_AUTH_FAIL",
                                                       self.kpi_measurements['failure_number'], msg.timestamp)
                                        upload_dict = {
                                            'total_number': self.kpi_measurements['total_number']['TOTAL'],
                                            'failure_number': self.kpi_measurements['failure_number']}
                                        self.upload_kpi('KPI.Retainability.AUTH_FAIL', upload_dict)
                                    else:
                                        self.log_warning("Unknown EMM cause: " + cause_idx)

        return 0





