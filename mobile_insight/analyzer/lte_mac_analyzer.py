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
        self.last_bytes = {} # LACI -> bytes <int> Last remaining bytes in MAC UL buffer
        self.buffer = {} # LCID -> [(sys_fn, sun_fn), packet_bytes] buffered mac ul packets
        self.ctrl_pkt_sfn = {} # [sys_fn, sun_fn] when last mac ul control packet comes
        self.cur_fn = None # Record current [sys_fn, sub_fn] for mac ul buffer

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
                    if log_item['Subpackets'][i].has_key('Samples'):
                        # print log_item
                        for sample in log_item['Subpackets'][i]['Samples']:
                            sub_fn = int(sample['Sub FN'])
                            sys_fn = int(sample['Sys FN'])
                            # Incorrect sys_fn and sub_fn are normally 1023 and 15
                            if not (sys_fn >= 1023 and sub_fn >= 9): # if the sys_fn and sub_fn are valid, update
                                if self.cur_fn:
                                    # reset historical data if time lag is bigger than 2ms
                                    lag = sys_fn * 10 + sub_fn - self.cur_fn[0] * 10 - self.cur_fn[1]
                                    if lag > 2 or -10238 < lag < 0:
                                        # print str(log_item['timestamp']), sys_fn, sub_fn, lag, self.cur_fn, self.buffer, self.ctrl_pkt_sfn
                                        self.last_bytes = {}
                                        self.buffer = {}
                                        self.ctrl_pkt_sfn = {}
                                self.cur_fn = [sys_fn, sub_fn]
                            elif self.cur_fn: # if invalid and inited, add current sfn
                                self.cur_fn[1] += 1
                                if self.cur_fn[1] == 10:
                                    self.cur_fn[1] = 0
                                    self.cur_fn[0] += 1
                                if self.cur_fn[0] == 1024:
                                    self.cur_fn = [0, 0]
                            if not self.cur_fn:
                                break

                            print str(log_item['timestamp']), self.cur_fn, self.buffer
                            for lcid in sample['LCIDs']:
                                idx = lcid['Ld Id']
                                new_bytes = int(lcid['New Compressed Bytes'])
                                ctrl_bytes = int(lcid['Ctrl bytes'])
                                total_bytes = int(lcid['Total Bytes'])
                                print new_bytes, ctrl_bytes, total_bytes

                                if idx not in self.buffer:
                                    self.buffer[idx] = []
                                if idx not in self.last_bytes:
                                    self.last_bytes[idx] = 0
                                if idx not in self.ctrl_pkt_sfn:
                                    self.ctrl_pkt_sfn[idx] = None

                                # add new packet to buffer
                                if not new_bytes == 0:
                                    # TODO: Need a better way to decided if it is a new packet or left packet
                                    if new_bytes > self.last_bytes[idx]:
                                        new_bytes = new_bytes - self.last_bytes[idx]
                                        self.buffer[idx].append([(self.cur_fn[0], self.cur_fn[1]), new_bytes])

                                if not ctrl_bytes == 0:
                                    total_bytes -= 2
                                    if not self.ctrl_pkt_sfn[idx]:
                                        self.ctrl_pkt_sfn[idx] = (self.cur_fn[0], self.cur_fn[1])
                                else:
                                    if self.ctrl_pkt_sfn[idx]:
                                        ctrl_pkt_delay = self.cur_fn[0] * 10 + self.cur_fn[1] \
                                                         - self.ctrl_pkt_sfn[idx][0] * 10 - self.ctrl_pkt_sfn[idx][1]
                                        ctrl_pkt_delay += 10240 if ctrl_pkt_delay < 0 else 0
                                        self.ctrl_pkt_sfn[idx] = None
                                        self.log_info(str(log_item['timestamp']) + " UL_CTRL_PKT_DELAY: " + str(ctrl_pkt_delay))

                                if self.last_bytes[idx] > total_bytes:
                                    sent_bytes = self.last_bytes[idx] - total_bytes
                                    while len(self.buffer[idx]) > 0 and sent_bytes > 0:
                                        pkt = self.buffer[idx][0]
                                        if pkt[1] <= sent_bytes:
                                            pkt_delay = self.cur_fn[0] * 10 + self.cur_fn[1] \
                                                             - pkt[0][0] * 10 - pkt[0][1]
                                            pkt_delay += 10240 if pkt_delay < 0 else 0
                                            self.buffer[idx].pop(0)
                                            sent_bytes -= pkt[1]
                                            self.log_info(str(log_item['timestamp']) + " UL_PKT_DELAY: " + str(pkt_delay))
                                        else:
                                            pkt[1] -= sent_bytes

                                self.last_bytes[idx] = total_bytes