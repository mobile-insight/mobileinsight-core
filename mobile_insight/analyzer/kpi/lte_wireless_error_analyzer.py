#!/usr/bin/python
# Filename: lte_wireless_error_analyzer.py
"""
lte_wireless_error_analyzer.py
An KPI analyzer to reveal Wireless errors （symbol errors, corruptions, etc.)

Author: Yuanjie Li
"""

__all__ = ["LteWirelessErrorAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer
import time


class LteWirelessErrorAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to reveal Wireless errors （symbol errors, corruptions, etc.)
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.current_kpi = {'SR_CONFIG_IDX': 0.00}

        for kpi in self.current_kpi:
            self.register_kpi("Wireless","SYMBOL_ERROR_RATE",self.__msg_callback)

        # add callback function
        self.add_source_callback(self.__msg_callback)

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
        #enable LTE RRC log
        source.enable_log("LTE_PHY_PDCCH_Decoding_Result")

    def __msg_callback(self, msg):

    	if msg.type_id == "LTE_PHY_PDCCH_Decoding_Result":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Hypothesis' in log_item_dict:
                for item in log_item_dict['Hypothesis']:
                    self._sym_error += item['Symbol Error Rate']
                self._sym_error_count += int(log_item_dict['Number of Hypothesis'])
            elif 'SF' in log_item_dict:
                for item in log_item_dict['SF']:
                    for item2 in item['Hypothesis']:
                        self._sym_error += item2['Symbol Error Rate']
                    self._sym_error_count += int(item['Number of Hypothesis'])



            cur_time = time.time()
            if cur_time - self._sym_error_timestamp >= self._report_period:
                kpi={}
                kpi['Symbol Error Rate'] = self._sym_error / self._sym_error_count
                # self.upload_kpi('KPI.Wireless.SYMBOL_ERROR_RATE', kpi)
                # self.broadcast_info('SYMBOL_ERROR_RATE', kpi)
                self.log_info("Symbol Err Rate: "+str(round(kpi['Symbol Error Rate'],2)))

                self._sym_error, self._sym_error_count = 0, 0
                self._sym_error_timestamp = cur_time

