#!/usr/bin/python3
# Filename: uplink_latency_analyzer.py
"""
uplink_latency_analyzer.py
An analyzer to monitor uplink packet waiting and processing latency
"""


__all__ = ["UplinkLatencyAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from mobile_insight.analyzer.analyzer import *


import time
import dis
import json
from datetime import datetime


# import threading


class UplinkLatencyAnalyzer(Analyzer):
    def __init__(self):
        Analyzer.__init__(self)
        self.add_source_callback(self.__msg_callback)

        # Timers 
        self.fn = -1
        self.sfn = -1

        # PHY stats
        self.cum_err_block = {0: 0, 1: 0}  # {0:xx, 1:xx} 0 denotes uplink and 1 denotes downlink
        self.cum_block = {0: 0, 1: 0}  # {0:xx, 1:xx} 0 denotes uplink and 1 denotes downlink

        # MAC buffer
        self.last_buffer = 0
        self.packet_queue = []

        # Stats
        self.all_packets = []
        self.tx_packets = []
        self.tmp_dict = {}

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        source.enable_log("LTE_PHY_PUSCH_Tx_Report")
        source.enable_log("LTE_MAC_UL_Buffer_Status_Internal")


    def __f_time_diff(self, t1, t2):
        if t1 > t2:
            t_diff = t2 + 10240 - t1
        else:
            t_diff = t2 - t1 + 1
        return t_diff

    def __f_time(self):
        return self.fn * 10 + self.sfn

    def __cmp_queues(self, type, data):
        if type == 1:
            for pkt in self.all_packets:
                if pkt[-2] == data[0]:
                    # print the stats

                    self.all_packets.remove(pkt)
                    return
            self.tx_packets.append(data)
        if type == 2:
            for pkt in self.tx_packets:
                if pkt[0] == data[-2]:
                    # print the stats
                    self.tx_packets.remove(pkt)
                    return
            self.all_packets.append(data)

    def __print_buffer(self):
        pass

    def __msg_callback(self, msg):

        if msg.type_id == "LTE_PHY_PUSCH_Tx_Report":
            log_item = msg.data.decode()
            if 'Records' in log_item:
                for record in log_item['Records']:
                    # print(record['PUSCH TB Size'])
                    retx_time = record['Current SFN SF']
                    if retx_time < 0:
                        retx_time += 1024

                    if record['Re-tx Index'] == 'First':
                        self.cum_block[0] += 1
                    else:
                        # print(record['Re-tx Index'])
                        self.cum_err_block[0] += 1

                        if retx_time in self.tmp_dict :
                            self.tmp_dict[retx_time]['Retx Latency'] = 8
                        else:
                            self.tmp_dict[retx_time] = {'Retx Latency': 8}
                    
                    for t in list(self.tmp_dict):
                        # print t, retx_time
                        # print self.tmp_dict
                        if (t < retx_time or (t > 1000 and retx_time < 20)):
                            if 'Retx Latency' not in self.tmp_dict[t]:
                                self.tmp_dict[t]['Retx Latency'] = 0
                            
                            if len(self.tmp_dict[t]) == 3:
                                print ('Waiting Latency:', self.tmp_dict[t]['Waiting Latency'], 'Tx Latency:', self.tmp_dict[t]['Tx Latency'], 'Retx Latency:', self.tmp_dict[t]['Retx Latency'])
                                self.all_packets.append(self.tmp_dict[t])
                                del(self.tmp_dict[t])




                    # self.__cmp_queues(1, (record['Current SFN SF'], record['Re-tx Index']))

        if msg.type_id == "LTE_MAC_UL_Buffer_Status_Internal":
            for packet in msg.data.decode()['Subpackets']:
                for sample in packet['Samples']:
                    SFN = sample['Sub FN']
                    FN = sample['Sys FN']
                    self.update_time(SFN, FN)
                    if (sample['LCIDs'] == []):
                        # print "error here!!"
                        continue
                    # print SFN, FN, self.sfn, self.fn
                    data = sample['LCIDs'][-1]
                    # print sample
                    
                    total_b = data['Total Bytes']
                    new_c = data['New Compressed Bytes']
                    retx_b = data['Retx bytes']
                    ctrl_b = data['Ctrl bytes']


                    # if (total_b > new_c) and ctrl_b == 0:

                    if total_b > self.last_buffer: 
                        # size, remaining buffer, incoming time, first byte time
                        self.packet_queue.append([total_b - self.last_buffer, total_b - self.last_buffer, self.__f_time(), -1])
                    elif total_b < self.last_buffer:
                        outgoing_bufer = self.last_buffer - total_b
                        while 1:
                            if self.packet_queue == []:
                                break
                            packet = self.packet_queue[0]
                            if packet[3] == -1:
                                packet[3] = self.__f_time()
                            if packet[1] > outgoing_bufer:
                                packet[1] -= outgoing_bufer
                                break
                            else:
                                # size, waiting latency, transmission latency
                                # print self.packet_queue, self.all_packets, outgoing_bufer
                                t_now = self.__f_time()
                                if (t_now not in self.tmp_dict):
                                    self.tmp_dict[t_now] = {}
                                self.tmp_dict[t_now]['Waiting Latency'] = self.__f_time_diff(packet[2], packet[3])
                                self.tmp_dict[t_now]['Tx Latency'] = self.__f_time_diff(packet[3], self.__f_time())
                                
                                # print [packet[0], self.__f_time_diff(packet[2], packet[3]), self.__f_time_diff(packet[2], self.__f_time())]

                                outgoing_bufer -= packet[1]
                                del self.packet_queue[0]
                                # self.__cmp_queues(2, (packet[0], self.__f_time_diff(packet[2], packet[3]), self.__f_time_diff(packet[2], t_now), t_now, self.last_buffer - new_c) )

                    self.last_buffer = total_b


    def update_time(self, SFN, FN):
        if self.sfn >= 0:      
            self.sfn += 1
            if self.sfn == 10:
                self.sfn = 0
                self.fn += 1
            if self.fn == 1024:
                self.fn = 0
        if SFN < 10:
            self.sfn = SFN
            self.fn = FN
