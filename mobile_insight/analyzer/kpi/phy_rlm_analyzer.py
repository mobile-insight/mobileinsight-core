#!/usr/bin/python
# Filename: phy_rlm_analyzer.py
"""
A LTE Phy Rlm analyzer.

Author: Yuanjie Li, Zhehui Zhang
"""

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer

__all__=["PhyRlmAnalyzer"]

class PhyRlmAnalyzer(KpiAnalyzer):

    """
    A protocol ananlyzer for LTE Radio Resource Control (RRC) protocol.
    """
    def __init__(self):

        KpiAnalyzer.__init__(self)

        #init internal states
        self.__rlm = {'in-sync bler': 0, 'out-sync bler': 0}    # [in-sync bler, out-sync bler] 
        self.add_source_callback(self.__rlm_filter)

        attributes=["in_sync_bler", "out_sync_bler"]

        self.register_kpi("Wireless", "BLER", self.__rlm_filter, 0)

    def __rlm_filter(self,msg):

        """
        Filter all LTE RRC packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """
        # log_item = msg.data
        if msg.type_id == "LTE_PHY_RLM_Report":
            log_item = msg.data.decode()
            log_item_dicts = dict(log_item)
            for log_item_dict in log_item_dicts['Records']:
                status_updated = False
                if self.__rlm['in-sync bler'] == 0 or self.__rlm['in-sync bler'] != log_item_dict["In Sync BLER (%)"] \
                        or self.__rlm['out-sync bler'] != log_item_dict["Out of Sync BLER (%)"]:
                    self.__rlm['in-sync bler'] = log_item_dict["In Sync BLER (%)"]
                    self.__rlm['out-sync bler'] = log_item_dict["Out of Sync BLER (%)"]

                    status_updated = True

                bler = max(self.__rlm['in-sync bler'], self.__rlm['out-sync bler'])
                if status_updated and bler > 0:
                    self.store_kpi("KPI_Wireless_BLER", '{:.2f}'.format(bler), log_item['timestamp'])

                    # self.log_info('LTE_RLM: ' + str(self.__rlm))
                    # self.broadcast_info('LTE_RLM', self.__rlm)
                    # self.upload_kpi("KPI.Wireless.BLER", self.__rlm)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE RLM messages.

        :param source: the trace source.
        :type source: trace collector
        """
        KpiAnalyzer.set_source(self,source)
        source.enable_log("LTE_PHY_RLM_Report")
