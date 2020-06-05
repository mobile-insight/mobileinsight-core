#!/usr/bin/python
# Filename: lte_qos_analyzer.py
"""
lte_qos_analyzer.py
An KPI analyzer to reveal the QoS configurations

Author: Yuanjie Li
"""

__all__ = ["LteQosAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer
import time


class LteQosAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to reveal Wireless errors （symbol errors, corruptions, etc.)
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.include_analyzer("LteNasAnalyzer", [self.__msg_callback])

        self.current_kpi = {'QCI': 0.00}

        for kpi in self.current_kpi:
            self.register_kpi("Configuration","QOS",self.__msg_callback)

        self._report_period = 1
        self._sym_error_timestamp = time.time()	
        self._sym_error = 0
        self._sym_error_count = 0



    def set_source(self,source):
        """
        Set the trace source. Enable the LTE RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        KpiAnalyzer.set_source(self,source)
        

    def __msg_callback(self, msg):

    	if msg.type_id == "LteNasAnalyzer":

            qos_profile = self.get_analyzer("LteNasAnalyzer").get_qos()
            # self.log_error(str(qos_profile.qci))

            if qos_profile.qci < 1 or qos_profile > 9:
                # Not standardized QCI
                return
            kpi = {}
            kpi['QCI'] = qos_profile.qci
            kpi['delay_class'] = qos_profile.delay_class
            kpi['reliability_class'] = qos_profile.reliability_class
            kpi['precedence_class'] = qos_profile.precedence_class
            kpi['peak_tput'] = qos_profile.peak_tput
            kpi['mean_tput'] = qos_profile.mean_tput
            kpi['traffic_class'] = qos_profile.traffic_class
            kpi['delivery_order'] = qos_profile.delivery_order
            kpi['transfer_delay'] = qos_profile.transfer_delay
            kpi['traffic_handling_priority'] = qos_profile.traffic_handling_priority
            kpi['max_bitrate_ulink'] = qos_profile.max_bitrate_ulink
            kpi['max_bitrate_dlink'] = qos_profile.max_bitrate_dlink
            kpi['guaranteed_bitrate_ulink'] = qos_profile.guaranteed_bitrate_ulink
            kpi['guaranteed_bitrate_dlink'] = qos_profile.guaranteed_bitrate_dlink
            kpi['max_bitrate_ulink_ext'] = qos_profile.max_bitrate_ulink_ext
            kpi['max_bitrate_dlink_ext'] = qos_profile.max_bitrate_dlink_ext
            kpi['guaranteed_bitrate_ulink_ext'] = qos_profile.guaranteed_bitrate_ulink_ext
            kpi['guaranteed_bitrate_dlink_ext'] = qos_profile.guaranteed_bitrate_dlink_ext
            kpi['residual_ber'] = qos_profile.residual_ber
            self.upload_kpi('KPI.Configuration.QOS', kpi)


