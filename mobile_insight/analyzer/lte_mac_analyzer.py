#!/usr/bin/python
# Filename: lte_mac_analyzer.py
"""
A 4G MAC-layer analyzer with the following functions

  - UL grant utilization analysis

  - TBA

Author: Yuanjie Li
"""


from mobile_insight.analyzer.analyzer import *
import datetime

__all__ = ["LteMacAnalyzer"]


class LteMacAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)
        self.buffer = []

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("LTE_MAC_UL_Tx_Statistics")
        source.enable_log("LTE_MAC_UL_Buffer_Status_Internal")

    def __msg_callback(self, msg):

        if msg.type_id == "LTE_MAC_UL_Tx_Statistics":
            log_item = msg.data.decode()

            grant_received = 0
            grant_utilized = 0
            grant_utilization = 0

            for i in range(0, len(log_item['Subpackets'])):
                grant_received += log_item['Subpackets'][i]['Sample']['Grant received']
                grant_utilized += log_item['Subpackets'][i]['Sample']['Grant utilized']

            if grant_received != 0:
                grant_utilization = round(
                    100.0 * grant_utilized / grant_received, 2)
                bcast_dict = {}
                bcast_dict['timestamp'] = str(log_item['timestamp'])
                bcast_dict['received'] = str(grant_received)
                bcast_dict['used'] = str(grant_utilized)
                bcast_dict['utilization'] = str(grant_utilization)
                self.broadcast_info("MAC_UL_GRANT", bcast_dict)
                self.log_info(str(log_item['timestamp']) +
                              " MAC UL grant: received=" +
                              str(grant_received) +
                              " bytes" +
                              " used=" +
                              str(grant_utilized) +
                              " bytes" +
                              " utilization=" +
                              str(grant_utilization) +
                              "%")

        elif msg.type_id == "LTE_MAC_UL_Buffer_Status_Internal":
            log_item = msg.data.decode()
            if log_item.has_key('Subpackets'):
                for i in range(0, len(log_item['Subpackets'])):
                    if log_item['Subpackets'][i].has_key('Sample'):
                        # TODO: deal with each buffer sample, note some fn and sfn are not correct.
                        pass

