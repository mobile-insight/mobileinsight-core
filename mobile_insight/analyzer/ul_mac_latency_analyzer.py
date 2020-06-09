#!/usr/bin/python
# Filename: ul_mac_latency_analyzer.py
"""
ul_latency_breakdown_analyzer.py
An analyzer to monitor mac layer waiting and processing latency

Author: Zhehui Zhang
"""

__all__ = ["UlMacLatencyAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET

from .analyzer import *

class UlMacLatencyAnalyzer(Analyzer):
    """
    An analyzer to monitor and manage uplink latency breakdown
    """
    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)
        self.last_bytes = {} # LACI -> bytes <int> Last remaining bytes in MAC UL buffer
        self.buffer = {} # LCID -> [(sys_fn, sun_fn), packet_bytes] buffered mac ul packets
        self.ctrl_pkt_sfn = {} # LCID -> [sys_fn, sun_fn] when last mac ul control packet comes
        self.cur_fn = None # Record current [sys_fn, sub_fn] for mac ul buffer
        self.lat_stat = [] # Record ul waiting latency (ts, sys_fn, sub_fn, pdu_size)
        self.queue_length = 0

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("LTE_MAC_UL_Buffer_Status_Internal")

    def __del_lat_stat(self):
        """
        Delete one lat_buffer after it is matched with rlc packet
        :return:
        """
        del self.lat_stat[0]

    def __msg_callback(self, msg):

        if msg.type_id == "LTE_MAC_UL_Buffer_Status_Internal":
            log_item = msg.data.decode()
            if 'Subpackets' in log_item:
                for i in range(0, len(log_item['Subpackets'])):
                    if 'Samples' in log_item['Subpackets'][i]:
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

                            for lcid in sample['LCIDs']:
                                idx = lcid['Ld Id']
                                #FIXME: Are these initializations valid?
                                if 'New Compressed Bytes' not in lcid:
                                    if 'New bytes' not in lcid:
                                        new_bytes = 0
                                    else:
                                        new_bytes = int(lcid['New bytes'])
                                else:
                                    new_bytes = int(lcid['New Compressed Bytes'])
                                ctrl_bytes = 0 if 'Ctrl bytes' not in lcid else int(lcid['Ctrl bytes'])
                                total_bytes = new_bytes + ctrl_bytes if 'Total Bytes' not in lcid else int(lcid['Total Bytes'])

                                # print 'total:', total_bytes

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
                                        # self.log_info(str(log_item['timestamp']) + " UL_CTRL_PKT_DELAY: " + str(ctrl_pkt_delay))
                                        
                                        bcast_dict = {}
                                        bcast_dict['timestamp'] = str(log_item['timestamp'])
                                        bcast_dict['delay'] = str(ctrl_pkt_delay)
                                        self.broadcast_info("UL_CTRL_PKT_DELAY", bcast_dict)

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
                                            self.lat_stat.append((log_item['timestamp'], \
                                                                 self.cur_fn[0], self.cur_fn[1], pkt[1], pkt_delay))
                                            # self.log_info(str(log_item['timestamp']) + " UL_PKT_DELAY: " + str(pkt_delay))
                                            bcast_dict = {}
                                            bcast_dict['timestamp'] = str(log_item['timestamp'])
                                            bcast_dict['delay'] = str(pkt_delay)
                                            self.broadcast_info("UL_PKT_DELAY", bcast_dict)
                                        else:
                                            pkt[1] -= sent_bytes
                                self.last_bytes[idx] = total_bytes

                            self.queue_length = sum(self.last_bytes.values()) 

                            # print self.last_bytes
                            # print self.lat_stat

