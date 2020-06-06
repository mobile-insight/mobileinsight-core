#!/usr/bin/python
# Filename: rrc_sr_analyzer.py
"""
rrc_sr_analyzer.py
A KPI analyzer to monitor and manage RRC connection success rate

Author: Zhehui Zhang
"""

__all__ = ["RrcSrAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer


class RrcSrAnalyzer(KpiAnalyzer):
    """
    A KPI analyzer to monitor and manage RRC connection success rate
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.cell_id = None

        self.kpi_measurements = {'success_number': {'EMERGENCY': 0, 'HIGH_PRIORITY_ACCESS': 0, \
                                                'MT_ACCESS': 0, 'MO_SIGNAL': 0, 'MO_DATA': 0, 'UNKNOWN': 0}, \
                            'total_number': {'EMERGENCY': 0, 'HIGH_PRIORITY_ACCESS': 0, \
                                                'MT_ACCESS': 0, 'MO_SIGNAL': 0, 'MO_DATA': 0, 'UNKNOWN': 0}, \
                                 'release_number': {'LB_TAU': 0, 'OTHER': 0, \
                                                    'CSFB': 0, 'SUSPEND': 0, 'UNKNOWN': 0}}

        self.register_kpi("Accessibility","RRC_SUC", self.__rrc_sr_callback,
                          list(self.kpi_measurements['success_number'].keys()))
        self.register_kpi("Accessibility", "RRC_REQ", self.__rrc_sr_callback,
                          list(self.kpi_measurements['total_number'].keys()))
        self.register_kpi("Retainability", "RRC_AB_REL", self.__rrc_sr_callback,
                          list(self.kpi_measurements['release_number'].keys()))
        self.register_kpi("Accessibility", "RRC_SR", self.__rrc_sr_callback)

        self.include_analyzer('UlMacLatencyAnalyzer', [self.__rrc_sr_callback])

        self.cause = None # record establishement casue of current RRC connection
        self.rrc_req_timestamp = None

        # add callback function
        self.add_source_callback(self.__rrc_sr_callback)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        KpiAnalyzer.set_source(self,source)
        #enable LTE RRC log
        source.enable_log("LTE_RRC_OTA_Packet")

    def __clear_kpi(self):
        self.kpi_measurements = {'success_number': {'EMERGENCY': 0, 'HIGH_PRIORITY_ACCESS': 0, \
                                                'MT_ACCESS': 0, 'MO_SIGNAL': 0, 'MO_DATA': 0, 'UNKNOWN': 0}, \
                            'total_number': {'EMERGENCY': 0, 'HIGH_PRIORITY_ACCESS': 0, \
                                                'MT_ACCESS': 0, 'MO_SIGNAL': 0, 'MO_DATA': 0, 'UNKNOWN': 0}, \
                                 'release_number': {'LB_TAU': 0, 'OTHER': 0, \
                                                    'CSFB': 0, 'SUSPEND': 0, 'UNKNOWN': 0}}

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

    def __rrc_sr_callback(self, msg):
        # deal with RRC OTA
        # cell_id = self.get_analyzer('TrackCellInfoAnalyzer').get_cur_cell_id()
        # if cell_id != self.cell_id :
            # self.cell_id = cell_id
            # self.__clear_counters()

        if msg.type_id == "LTE_RRC_OTA_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                for field in log_xml.iter('field'):

                    if field.get('name') == "lte-rrc.rrcConnectionSetupComplete_element":
                        # self.__clear_kpi()
                        if self.cause:
                            self.kpi_measurements['success_number'][self.cause] += 1
                            self.cause = None
                            self.store_kpi("KPI_Accessibility_RRC_SUC", self.kpi_measurements['total_number'], log_item_dict['timestamp'])
                            # upload_dict = dict((k, self.kpi_measurements[k]) for k in ('success_number', 'total_number'))
                            # self.upload_kpi('KPI.Accessibility.RRC_SR', upload_dict, log_item_dict['timestamp'])
                            # self.upload_kpi('KPI.Accessibility.RRC_SR', upload_dict)
                            # self.log_debug(str(upload_dict))
                            # self.broadcast_info('RRC_SR', upload_dict)

                            # print self.local_query_kpi('KPI_Accessibility_RRC_SR_MO_DATA', msg.timestamp)

                            # Upload RRC latency
                            # delta = (log_item_dict['timestamp'] - self.rrc_req_timestamp).total_seconds()
                            # upload_dict = {'latency': delta}
                            # self.upload_kpi("KPI.Accessibility.RRC_LATENCY", upload_dict)
                            # self.log_debug(str(upload_dict))


                    elif field.get('name') ==  "lte-rrc.rrcConnectionRequest_element":
                    	self.rrc_req_timestamp = log_item_dict['timestamp']
                    elif field.get('name') == 'lte-rrc.establishmentCause':
                        # self.__clear_kpi()
                        if field.get('show') == '4':
                            self.cause = 'MO_DATA'
                            self.kpi_measurements['total_number'][self.cause] += 1
                        elif field.get('show') == '3':
                            self.cause = 'MO_SIGNAL'
                            self.kpi_measurements['total_number'][self.cause] += 1
                        elif field.get('show') == '2':
                            self.cause = 'MT_ACCESS'
                            self.kpi_measurements['total_number'][self.cause] += 1
                        elif field.get('show') == '1':
                            self.cause = 'HIGH_PRIORITY_ACCESS'
                            self.kpi_measurements['total_number'][self.cause] += 1
                        elif field.get('show') == '0':
                            self.cause = 'EMERGENCY'
                            self.kpi_measurements['total_number'][self.cause] += 1
                        else:
                            self.cause = 'UNKNOWN'
                            self.kpi_measurements['total_number'][self.cause] += 1
                            #FIXME: MobileInsight crashes after reporting this warning
                            self.log_warning("Unknown lte-rrc.establishmentCause: " + str(field.get('showname')))
                        self.store_kpi("KPI_Accessibility_RRC_REQ", self.kpi_measurements['total_number'], log_item_dict['timestamp'])

                    elif field.get('name') == 'lte-rrc.releaseCause':
                        # TODO: check if this release is abnormal
                        # self.__clear_kpi()
                        queue_length = self.get_analyzer('UlMacLatencyAnalyzer').queue_length
                        if queue_length != 0:
                            # Check cause loadBalancingTAUrequired, other, cs-FallbackHighPriority-v1020, rrc-Suspend-v1320
                            if field.get('show') == '0': # Cause: Other
                                self.kpi_measurements['release_number']['LB_TAU'] += 1
                            elif field.get('show') == '1': # Cause: Other
                                self.kpi_measurements['release_number']['OTHER'] += 1
                            elif field.get('show') == '2': # Cause: Other
                                self.kpi_measurements['release_number']['CSFB'] += 1
                            elif field.get('show') == '3': # Cause: Other
                                self.kpi_measurements['release_number']['SUSPEND'] += 1
                            else:
                                #FIXME: MobileInsight crashes after reporting this warning
                                self.kpi_measurements['release_number']['UNKNOWN'] += 1
                                self.log_warning("Unknown lte-rrc.releaseCause: "+ field.get('showname'))
                            self.store_kpi("KPI_Retainability_RRC_AB_REL", self.kpi_measurements['release_number'], log_item_dict['timestamp'])
                            # upload_dict = {'total_number': sum(self.kpi_measurements['total_number'].values()),
                                           # 'release_number': self.kpi_measurements['release_number']}
                            # self.upload_kpi('KPI.Retainability.RRC_AB_REL', upload_dict, log_item_dict['timestamp'])
                            # self.upload_kpi('KPI.Retainability.RRC_AB_REL', upload_dict)
                            # self.log_info(str(upload_dict))
                            # self.broadcast_info('RRC_SR', upload_dict)
        return 0





